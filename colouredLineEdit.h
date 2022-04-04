#ifndef COLOURED_LINE_EDITOR_H
#define COLOURED_LINE_EDIT_H

#include <QLineEdit>
#include <QColor>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QApplication>

class ColouredLineEdit
    : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QColor colorText READ getColorText WRITE setColorText)
    Q_PROPERTY(QColor colorBackground READ getColorBackground WRITE setColorBackground)

public:
    explicit ColouredLineEdit(QWidget *) noexcept;

    QColor getColorText() const noexcept;
    QColor getColorBackground() const noexcept;

    void setColorText(const QColor &) noexcept;
    void setColorBackground(const QColor &) noexcept;
    void setIntermediateColor(const QColor &) noexcept;

public slots:
    void slotStartAnimation() noexcept;
    void slotRedNotice() noexcept;
    void slotGreenNotice() noexcept;

private:
    QString rgb(unsigned char, unsigned char, unsigned char) const noexcept;
    QString rgb(QColor) const noexcept;
    QString qss()const noexcept;

private:
    static constexpr std::string_view m_rgb = {
        "rgb(%1, %2, %3)"
    };
    static constexpr std::string_view m_billet = {
        "background-color: %1;"
        "color: %2;"
    };

    QColor m_colorText;
    QColor m_colorBackground;
    QParallelAnimationGroup m_colorAnimation;
    QPropertyAnimation m_textAnimation;
    QPropertyAnimation m_backgroundAnimation;
};

#endif // COLOURED_LINE_EDITOR_H
