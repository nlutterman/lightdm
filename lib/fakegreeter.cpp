/*
This file is part of LightDM-KDE.

Copyright 2012 David Edmundson <kde@davidedmundson.co.uk>

LightDM-KDE is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LightDM-KDE is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with LightDM-KDE.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "fakegreeter.h"

#include <KDebug>

#include <QApplication>
#include <QTimer>

FakeGreeter::FakeGreeter(QObject *parent) :
    QLightDM::Greeter(parent)
{
    m_isAuthenticated = false;
}

FakeGreeter::~FakeGreeter()
{

}

QString FakeGreeter::lastLoggedInUser() const
{
    return QString();
}

QString FakeGreeter::guestLoginName() const
{
    return QLatin1String("*guest");
}

bool FakeGreeter::isAuthenticated() const
{
    return m_isAuthenticated;
}

bool FakeGreeter::connectSync()
{
    return true;
}

void FakeGreeter::authenticate(const QString &username)
{
    kDebug() << "authenticating as " << username;
    //emit showPrompt in 4 seconds
    QTimer::singleShot(4*1000, this, SLOT(onAuthenticationTimerExpired()));
}

void FakeGreeter::authenticateAsGuest()
{
    Q_EMIT authenticationComplete();
}

void FakeGreeter::respond(const QString &response)
{
    Q_UNUSED(response)
    m_isAuthenticated = true;
    Q_EMIT authenticationComplete();
}

bool FakeGreeter::startSessionSync(const QString &session)
{
    kDebug() << "starting session " << session;
    QApplication::instance()->quit();
    return true;
}

void FakeGreeter::onAuthenticationTimerExpired()
{
    Q_EMIT showPrompt("Password:", QLightDM::Greeter::PromptTypeQuestion);
}
