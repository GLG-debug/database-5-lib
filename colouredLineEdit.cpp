#include "colouredLineEdit.h"

ColouredLineEdit::ColouredLineEdit(QWidget *parent) noexcept
    : QLineEdit( parent )
    , m_colorText( Qt::black )
    , m_colorBackground( Qt::white )
    , m_textAnimation( this, "colorText" )
    , m_backgroundAnimation( this, "colorBackground" )
{
    m_textAnimation.setEndValue( m_colorText );
    m_backgroundAnimation.setEndValue( m_colorBackground );

    m_textAnimation.setDuration( 2000 );
    m_backgroundAnimation.setDuration( 1000 );

    m_textAnimation.setEasingCurve( QEasingCurve::InCirc );
    m_backgroundAnimation.setEasingCurve( QEasingCurve::OutExpo );

    for (auto animation : { &m_textAnimation, &m_backgroundAnimation } ) {
        m_colorAnimation.addAnimation( animation );
    }
}

QColor ColouredLineEdit::getColorText() const noexcept {
    return m_colorText;
}

QColor ColouredLineEdit::getColorBackground() const noexcept {
    return m_colorBackground;
}

void ColouredLineEdit::setColorText(const QColor &color) noexcept {
    m_colorText = color;
    setStyleSheet( qss() );
}

void ColouredLineEdit::setColorBackground(const QColor &color) noexcept {
    m_colorBackground = color;
    setStyleSheet( qss() );
}

void ColouredLineEdit::setIntermediateColor(const QColor &color) noexcept {
    m_textAnimation.setStartValue(color);
    m_backgroundAnimation.setKeyValueAt(0.5f, color);
}

void ColouredLineEdit::slotStartAnimation() noexcept {
    m_colorAnimation.stop();
    m_colorAnimation.start();
}

void ColouredLineEdit::slotRedNotice() noexcept {
    setIntermediateColor( Qt::red );
    slotStartAnimation();
}

void ColouredLineEdit::slotGreenNotice() noexcept {
    setIntermediateColor( Qt::darkGreen );
    slotStartAnimation();
}

QString ColouredLineEdit::rgb(unsigned char r, unsigned char g, unsigned char b) const noexcept {
    QString result( m_rgb.data() );
    return result.arg(r).arg(g).arg(b);
}

QString ColouredLineEdit::rgb(QColor color) const noexcept {
    return rgb( color.red(), color.green(), color.blue() );
}

QString ColouredLineEdit::qss() const noexcept {
    QString result( m_billet.data() );
    return result.arg( rgb(m_colorBackground), rgb(m_colorText) );
}
