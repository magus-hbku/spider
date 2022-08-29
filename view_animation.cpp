#include "view_animation.h"
#include <QMatrix4x4>


QVariant QVector3DAnimation::interpolated(const QVariant &from, const QVariant &to, qreal progress) const
{
    QVector3D src = from.value<QVector3D>();
    QVector3D dst = to.value<QVector3D>();
    QVector3D output = src + progress * (dst- src);
    return output;
}


view_animation::view_animation()
{
    m_duration = 10000;

    m_eye_animation = new QVector3DAnimation();
    m_eye_animation->setEasingCurve(QEasingCurve::Linear);
    m_eye_animation->setStartValue(0);
    m_eye_animation->setLoopCount(-1);

    m_target_animation = new QVector3DAnimation();
    m_target_animation->setEasingCurve(QEasingCurve::Linear);
    m_target_animation->setStartValue(0);
    m_target_animation->setLoopCount(-1);

    m_active =  false;
    reset();
}

view_animation::~view_animation() {
    delete m_eye_animation;
    delete m_target_animation;
}

void view_animation::push_key_frame(const QVector3D& eye, const QVector3D& target)
{
    m_eye_vector.push_back(eye);
    m_target_vector.push_back(target);
    update_animation();
}

void view_animation::reset()
{
    m_eye_vector.clear();
    m_target_vector.clear();
    m_eye_vector.push_back( QVector3D(0.0,0.0,0.0));
    m_target_vector.push_back( QVector3D(0.0,0.0,-1.0));
}


void view_animation::start()
{
    if (m_eye_vector.size() < 2) return;
    m_active = true;
    m_eye_animation->start();
    m_target_animation->start();
}

void view_animation::pause()
{
    m_active = false;
    m_eye_animation->pause();
    m_target_animation->pause();
}


void view_animation::resume()
{
    m_active = true;
    m_eye_animation->resume();
    m_target_animation->resume();
}

void view_animation::stop() {
    m_active = false;
    m_eye_animation->stop();
    m_target_animation->stop();
}



void view_animation::update_animation() {

    m_eye_animation->setEndValue(m_duration);
    m_eye_animation->setDuration(m_duration);
    m_target_animation->setEndValue(m_duration);
    m_target_animation->setDuration(m_duration);

    std::size_t n_steps  = m_eye_vector.size();
    QVector< QVector3DAnimation::KeyValue> eye_values;
    QVector< QVector3DAnimation::KeyValue> target_values;
    for (std::size_t n = 0; n < n_steps; ++n) {
        qreal step = qreal(n) / qreal(n_steps -1);
        eye_values.push_back( QVector3DAnimation::KeyValue(step, m_eye_vector[n]) );
        target_values.push_back( QVector3DAnimation::KeyValue(step, m_target_vector[n]) );
    }
    m_eye_animation->setKeyValues( eye_values);
    m_target_animation->setKeyValues( target_values );
}


void view_animation::current_camera_in(QMatrix4x4& m)
{
    QVector3D e = m_eye_animation->currentValue().value<QVector3D>();
    QVector3D t = m_target_animation->currentValue().value<QVector3D>();
    m.lookAt(e,  t, QVector3D(0, 1, 0));
}

