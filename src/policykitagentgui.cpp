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

#include <QIcon>
#include "policykitagentgui.h"
#include <unistd.h>

namespace LXQtPolicykit
{


PolicykitAgentGUI::PolicykitAgentGUI(const QString &actionId,
                                     const QString &message,
                                     const QString &iconName,
                                     const PolkitQt1::Details &details,
                                     const PolkitQt1::Identity::List &identities)
   : QDialog(nullptr, Qt::WindowStaysOnTopHint)
{
    setupUi(this);
    Q_UNUSED(actionId);
    Q_UNUSED(details); // it seems too confusing for end user (=me)

    messageLabel->setText(message);
    QIcon icon = QIcon::fromTheme(iconName);
    if (icon.isNull())
        icon = QIcon::fromTheme(QStringLiteral("dialog-question"));
    iconLabel->setPixmap(icon.pixmap(64, 64));

    const uid_t current_uid = getuid();
    int current_user_index = -1;
    for (const PolkitQt1::Identity& identity : identities)
    {
        const int i = identityComboBox->count(); // index of the added item
        identityComboBox->addItem(identity.toString());
        PolkitQt1::UnixUserIdentity const * const u_id = static_cast<const PolkitQt1::UnixUserIdentity *>(&identity);
        if (u_id != nullptr && u_id->uid() == current_uid)
            current_user_index = i;
    }
    if (current_user_index != -1)
        identityComboBox->setCurrentIndex(current_user_index);
    connect(identityComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &PolicykitAgentGUI::onIdentityChanged);
    passwordEdit->setFocus(Qt::OtherFocusReason);
}

PolicykitAgentGUI::~PolicykitAgentGUI() = default;

void PolicykitAgentGUI::setPromptLabel(const QString &text)
{
    if (QString::compare(text.trimmed(), QLatin1StringView("Password:"), Qt::CaseInsensitive) == 0)
        promptLabel->setText(QCoreApplication::translate("PolicykitAgentGUI", "Password:"));
    else
        promptLabel->setText(text);
}

void PolicykitAgentGUI::setPrompt(const PolkitQt1::Identity &identity, const QString &text, bool echo)
{
    const int ix = identityComboBox->findText(identity.toString());
    if (ix != -1)
    {
        identityComboBox->setItemData(ix, text, RolePromptText);
        identityComboBox->setItemData(ix, echo, RolePromptEcho);

        if (ix == identityComboBox->currentIndex())
        {
            setPromptLabel(text);
            passwordEdit->setEchoMode(echo ? QLineEdit::Normal : QLineEdit::Password);
        }
    }
}

QString PolicykitAgentGUI::identity()
{
    Q_ASSERT(identityComboBox->currentIndex() != -1);
    return identityComboBox->currentText();
}

QString PolicykitAgentGUI::response() {
  QString response = passwordEdit->text();
  passwordEdit->setText(QString());
  return response;
}

void PolicykitAgentGUI::onIdentityChanged(int index)
{
    QVariant text = identityComboBox->itemData(index, RolePromptText);
    QVariant echo = identityComboBox->itemData(index, RolePromptEcho);
    if (text != QVariant{})
        setPromptLabel(text.toString());
    if (echo != QVariant{})
        passwordEdit->setEchoMode(echo.toBool() ? QLineEdit::Normal : QLineEdit::Password);
}

} // namespace
