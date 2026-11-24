/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2011-2012 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1

#include <polkitagent/polkitagent.h>
#include <PolkitQt1/Subject>

#include <QMessageBox>

#include "policykitagent.h"
#include "policykitagentgui.h"


namespace LXQtPolicykit
{

namespace
{
constexpr int maximumAuthenticationAttempts = 3;
}

PolicykitAgent::PolicykitAgent(QObject *parent)
    : PolkitQt1::Agent::Listener(parent),
      m_inProgress(false),
      m_inProgressAlert(false),
      m_userCancelled(false),
      m_errorShown(false),
      m_infoShown(false),
      m_authenticationAttempts(0),
      m_gui(nullptr)
{
    PolkitQt1::UnixSessionSubject session(getpid());
    registerListener(session, QStringLiteral("/org/lxqt/PolicyKit1/AuthenticationAgent"));
}

PolicykitAgent::~PolicykitAgent()
{
    if (m_gui != nullptr)
    {
        m_gui->blockSignals(true);
        m_gui->deleteLater();
    }
    deleteSessions();
}

void PolicykitAgent::deleteSessions()
{
    for (auto i = m_SessionIdentity.begin(), i_e = m_SessionIdentity.end(); i != i_e; ++i)
        delete i.key();
    m_SessionIdentity.clear();
}

void PolicykitAgent::createSession(const PolkitQt1::Identity &identity,
                                   const QString &cookie,
                                   PolkitQt1::Agent::AsyncResult *result)
{
    auto *session = new PolkitQt1::Agent::Session(identity, cookie, result);
    m_SessionIdentity[session] = identity;
    connect(session, &PolkitQt1::Agent::Session::request, this, &PolicykitAgent::request);
    connect(session, &PolkitQt1::Agent::Session::completed, this, &PolicykitAgent::completed);
    connect(session, &PolkitQt1::Agent::Session::showError, this, &PolicykitAgent::showError);
    connect(session, &PolkitQt1::Agent::Session::showInfo, this, &PolicykitAgent::showInfo);
    session->initiate();
}


void PolicykitAgent::initiateAuthentication(const QString &actionId,
                                            const QString &message,
                                            const QString &iconName,
                                            const PolkitQt1::Details &details,
                                            const QString &cookie,
                                            const PolkitQt1::Identity::List &identities,
                                            PolkitQt1::Agent::AsyncResult *result)
{
    if (m_inProgress)
    {
	const QString & info = tr("Another authentication is in progress. Please try again later.");
        if (!m_inProgressAlert) {
            m_inProgressAlert = true;
	    QMessageBox::information(nullptr, tr("PolicyKit Information"), info);

            m_inProgressAlert = false;
        }
        result->setError(info);
        result->setCompleted();
        return;
    }
    m_inProgress = true;
    m_userCancelled = false;
    m_errorShown = false;
    m_infoShown = false;
    m_authenticationAttempts = 0;
    m_lastError.clear();
    m_cookie = cookie;
    deleteSessions();

    if (m_gui != nullptr)
    {
        delete m_gui;
        m_gui = nullptr;
    }
    m_gui = new PolicykitAgentGUI(actionId, message, iconName, details, identities);

    for(const PolkitQt1::Identity& i : identities)
    {
        createSession(i, cookie, result);
    }
}

bool PolicykitAgent::initiateAuthenticationFinish()
{
    // dunno what are those for...
    m_inProgress = false;
    m_cookie.clear();
    return true;
}

void PolicykitAgent::cancelAuthentication()
{
    // dunno what are those for...
    m_inProgress = false;
    m_cookie.clear();
}

void PolicykitAgent::request(const QString &request, bool echo)
{
    PolkitQt1::Agent::Session *session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    Q_ASSERT(session);
    Q_ASSERT(m_gui);

    // PAM may still ask for a password after showInfo (e.g. account locked); don't.
    if (m_infoShown) {
        session->cancel();
        return;
    }

    PolkitQt1::Identity identity = m_SessionIdentity[session];
    m_gui->setPrompt(identity, request, echo);
    connect(m_gui, &QDialog::finished, session, [this, session] (int result)
    {
        if (result == QDialog::Accepted)
        {
            if (m_gui->identity() == m_SessionIdentity[session].toString())
                session->setResponse(m_gui->response());
            else
                session->cancel();
        }
        else {
            m_userCancelled = true;
            session->cancel();
        }
    }, Qt::SingleShotConnection);
    m_gui->show();
    m_gui->activateWindow();
    m_gui->raise();
}

void PolicykitAgent::completed(bool gainedAuthorization)
{
    PolkitQt1::Agent::Session * session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    Q_ASSERT(session);
    Q_ASSERT(m_gui);

    const PolkitQt1::Identity identity = m_SessionIdentity.value(session);
    const bool selectedIdentity = m_gui->identity() == identity.toString();
    PolkitQt1::Agent::AsyncResult *result = session->result();

    if (m_inProgress && selectedIdentity)
    {
        if (!gainedAuthorization && !m_userCancelled && !m_infoShown
            && ++m_authenticationAttempts < maximumAuthenticationAttempts)
        {
            if (!m_errorShown)
                QMessageBox::information(nullptr, tr("Authorization Failed"), tr("Authentication failed. Please try again."));

            m_errorShown = false;
            m_infoShown = false;
            m_lastError.clear();
            // A completed Polkit session cannot be reused; start a new PAM conversation.
            createSession(identity, m_cookie, result);
            return;
        }

        if (!gainedAuthorization && !m_userCancelled && !m_errorShown)
        {
            const QString text = m_lastError.isEmpty()
                ? tr("Authentication failed")
                : m_lastError;
            QMessageBox::information(nullptr, tr("Authorization Failed"), text);
        }

        // Note: the setCompleted() must be called exacly once (as the
        // AsyncResult is shared by all the sessions)
        result->setCompleted();
        m_inProgress = false;
        m_cookie.clear();
    }
}

void PolicykitAgent::showError(const QString &text)
{
    m_lastError = text;
    m_errorShown = true;
    QMessageBox::warning(nullptr, tr("PolicyKit Error"), text);
}

void PolicykitAgent::showInfo(const QString &text)
{
    m_lastError = text;
    m_errorShown = true;
    m_infoShown = true;
    // Blocking so callers only see their error after the user dismisses this.
    QMessageBox::information(nullptr, tr("PolicyKit Information"), text);
}

} //namespace
