#pragma once

#include <QObject>
#include <QSettings>
#include <QString>

// Resolves the API key to use: a user-entered key always wins; otherwise
// falls back to a build-time default (baked in only for OpenRepos/sideload
// builds via TREINPLANNER_DEFAULT_API_KEY - Harbour builds never define it,
// so hasDefaultKey is false and the user must supply their own key).
class ApiKeyManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userKey READ userKey WRITE setUserKey NOTIFY userKeyChanged)
    Q_PROPERTY(bool hasDefaultKey READ hasDefaultKey CONSTANT)
    Q_PROPERTY(bool hasUsableKey READ hasUsableKey NOTIFY userKeyChanged)

public:
    explicit ApiKeyManager(QObject *parent = nullptr);

    QString userKey() const;
    void setUserKey(const QString &key);

    bool hasDefaultKey() const;
    bool hasUsableKey() const;

    // The key to actually send with requests: userKey() if set, else the
    // baked-in default (which is empty on Harbour builds).
    QString effectiveKey() const;

signals:
    void userKeyChanged();

private:
    QSettings m_settings;
};
