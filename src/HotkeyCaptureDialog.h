#pragma once

#include <QDialog>
#include <QKeySequence>

class QLabel;

class HotkeyCaptureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HotkeyCaptureDialog(QWidget *parent = nullptr);
    QKeySequence getHotkey() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void updateLabel();

    QLabel *m_label;
    QList<int> m_keys;
    QKeySequence m_keySequence;
};
