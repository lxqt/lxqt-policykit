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

#ifndef POLICYKITAGENTGUI_H
#define POLICYKITAGENTGUI_H

#include <PolkitQt1/Details>
#include <PolkitQt1/Identity>

#include "ui_policykitagentgui.h"

namespace LXQtPolicykit
{

class PolicykitAgentGUI : public QDialog, public Ui::PolicykitAgentGUI
{
    Q_OBJECT

private:
    enum DataRoles
    {
        RolePromptText = Qt::UserRole
            , RolePromptEcho
    };

    void setPromptLabel(const QString &text);

public:
    PolicykitAgentGUI(const QString &actionId,
                      const QString &message,
                      const QString &iconName,
                      const PolkitQt1::Details &details,
                      const PolkitQt1::Identity::List &identities);
    ~PolicykitAgentGUI() override;

    void setPrompt(const PolkitQt1::Identity &identity, const QString &text, bool echo);
    /*! \brief Returns currently selected identity (serialized by toString())
     */
    QString identity();
    QString response();

public slots:
    void onIdentityChanged(int index);

};

} // namespace

#endif
