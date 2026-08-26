#include "ApiKeyManager.h"

#ifndef TREINPLANNER_DEFAULT_API_KEY
#define TREINPLANNER_DEFAULT_API_KEY ""
#endif

namespace {
const char *kUserKeySetting = "apiKey/userKey";
}

ApiKeyManager::ApiKeyManager(QObject *parent)
    : QObject(parent)
{
}

QString ApiKeyManager::userKey() const
{
    return m_settings.value(QLatin1String(kUserKeySetting)).toString();
}

void ApiKeyManager::setUserKey(const QString &key)
{
    if (userKey() == key)
        return;
    m_settings.setValue(QLatin1String(kUserKeySetting), key);
    emit userKeyChanged();
}

bool ApiKeyManager::hasDefaultKey() const
{
    return !QStringLiteral(TREINPLANNER_DEFAULT_API_KEY).isEmpty();
}

bool ApiKeyManager::hasUsableKey() const
{
    return !effectiveKey().isEmpty();
}

QString ApiKeyManager::effectiveKey() const
{
    const QString user = userKey();
    if (!user.isEmpty())
        return user;
    return QStringLiteral(TREINPLANNER_DEFAULT_API_KEY);
}
