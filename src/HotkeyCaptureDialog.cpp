#include "HotkeyCaptureDialog.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QKeyEvent>

HotkeyCaptureDialog::HotkeyCaptureDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Assign Hotkey"));
    setFixedSize(300, 100);

    m_label = new QLabel(tr("Press a key combination..."), this);
    m_label->setAlignment(Qt::AlignCenter);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_label);
    setLayout(layout);
}

QKeySequence HotkeyCaptureDialog::getHotkey() const
{
    return m_keySequence;
}

void HotkeyCaptureDialog::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Alt || key == Qt::Key_Meta) {
        // Это просто клавиша-модификатор, добавляем ее и ждем основную клавишу
        if (!m_keys.contains(key)) {
            m_keys.append(key);
        }
    } else {
        // Это основная клавиша, добавляем ее и все текущие модификаторы
        int modifiers = event->modifiers();
        if (modifiers & Qt::ControlModifier && !m_keys.contains(Qt::Key_Control)) m_keys.prepend(Qt::Key_Control);
        if (modifiers & Qt::ShiftModifier && !m_keys.contains(Qt::Key_Shift)) m_keys.prepend(Qt::Key_Shift);
        if (modifiers & Qt::AltModifier && !m_keys.contains(Qt::Key_Alt)) m_keys.prepend(Qt::Key_Alt);
        if (modifiers & Qt::MetaModifier && !m_keys.contains(Qt::Key_Meta)) m_keys.prepend(Qt::Key_Meta);
        
        if (!m_keys.contains(key)) {
            m_keys.append(key);
        }
        
        // Как только нажата основная клавиша, мы считаем комбинацию завершенной
        m_keySequence = QKeySequence(m_keys.size() > 0 ? m_keys[0] : 0, m_keys.size() > 1 ? m_keys[1] : 0, m_keys.size() > 2 ? m_keys[2] : 0, m_keys.size() > 3 ? m_keys[3] : 0);
        accept();
    }
    updateLabel();
}

void HotkeyCaptureDialog::keyReleaseEvent(QKeyEvent *event)
{
    // Теперь мы принимаем хоткей по нажатию основной клавиши, а не по отпусканию.
    // Это более интуитивно для пользователя.
    Q_UNUSED(event);
}

void HotkeyCaptureDialog::updateLabel()
{
    if (m_keys.isEmpty()) {
        m_label->setText(tr("Press a key combination..."));
    } else {
        QKeySequence seq(m_keys.size() > 0 ? m_keys[0] : 0, m_keys.size() > 1 ? m_keys[1] : 0, m_keys.size() > 2 ? m_keys[2] : 0, m_keys.size() > 3 ? m_keys[3] : 0);
        m_label->setText(seq.toString(QKeySequence::NativeText));
    }
}
