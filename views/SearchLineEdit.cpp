#include "SearchLineEdit.h"

/**
 * \class SearchLineEdit
 * \brief A QLineEdit emitting textEdited() signal also on printable key
 * releases.
 */

/**
 * @brief Creates the QLineEdit and sets the placeholder text.
 * @param parent Optional parent QWidget.
 */
SearchLineEdit::SearchLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    setPlaceholderText("Search");
}

/**
 * @brief Remembers the text before the key press changes it for future
 * comparison by keyReleaseEvent().
 */
void SearchLineEdit::keyPressEvent(QKeyEvent *event)
{
    m_before_key = text();
    QLineEdit::keyPressEvent(event);
}

/**
 * \brief If the text remembered by keyPressEvent() is different from the
 * current text, emits textEdited().
 * \bug Changing the default and expected behavior of textEdited signal
 * is bad. I should connect this signal and the default textEdited signal
 * to a signal to be used from outside.
 */
void SearchLineEdit::keyReleaseEvent(QKeyEvent *event)
{
    if (m_before_key != text()) {
        emit textEdited(text());
    }
    QLineEdit::keyReleaseEvent(event);
}
