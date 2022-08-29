#ifndef VIEW_ANIMATION_H
#define VIEW_ANIMATION_H

#include <QVariantAnimation>
#include <QVector3D>


class QVector3DAnimation: public  QVariantAnimation
{
public:
    QVector3DAnimation(): QVariantAnimation() {};
    virtual QVariant interpolated(const QVariant &from, const QVariant &to, qreal progress) const;
};


class view_animation
{
public:
    view_animation();
    ~view_animation();

    const bool& active() const { return m_active; }

    void start();
    void pause();
    void resume();
    void stop();
    int& duration() { return m_duration; }
    const int& duration() const { return m_duration; }

    void push_key_frame(const QVector3D& eye, const QVector3D& target);
    void reset();
    void current_camera_in(QMatrix4x4& m);
    QVector3D  current_eye() const { return m_eye_animation->currentValue().value<QVector3D>();}
    QVector3D  current_target() const { return m_target_animation->currentValue().value<QVector3D>();}

protected:
    void update_animation();

protected:
    bool m_active;
    int m_duration;

    QVector<QVector3D> m_eye_vector;
    QVector<QVector3D> m_target_vector;

    QVector3DAnimation* m_eye_animation;
    QVector3DAnimation* m_target_animation;
};

#endif // VIEW_ANIMATION_H
