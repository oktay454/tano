#include <QMouseEvent>
#include <QDebug>
#include <QApplication>

#include "Video.h"

Video::Video(QWidget *parent)
    : VideoWidget(parent)
{
	channel = new Phonon::MediaObject(parent);
	audio = new Phonon::AudioOutput(Phonon::VideoCategory, parent);
	aslider = new Phonon::VolumeSlider;
	aslider->setIconSize(QSize(32,32));
	aslider->setAudioOutput(audio);
	Phonon::createPath(channel, this);
	Phonon::createPath(channel, audio);

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(hideMouse()));

	audio->setVolume(0.5);
	qDebug() << "Volume:" << audio->volume()*100;

	pos = QPoint();

	move = true;
}


Video::~Video()
{

}

//Events:
void Video::mouseDoubleClickEvent(QMouseEvent *event)
{
	event->ignore();
	emit full();
}
void Video::mouseMoveEvent(QMouseEvent *event)
{
	event->ignore();

	if(this->isFullScreen() && event->globalPos() != pos && move) {
		qApp->setOverrideCursor(Qt::ArrowCursor);
		timer->start(1000);
		emit mouseMove();
		pos = event->globalPos();
	}
}
void Video::mousePressEvent(QMouseEvent *event)
{
	event->ignore();

	if(event->button() == Qt::RightButton)
		emit rightClick(event->globalPos());
}
void Video::wheelEvent(QWheelEvent *event)
{
	event->ignore();

	if(event->delta()>0)
		emit wheel(true);
	else
		emit wheel(false);
}

void Video::hideMouse()
{
	if(this->isFullScreen() && move) {
		qApp->setOverrideCursor(Qt::BlankCursor);
		timer->stop();
	}
}

//Public functions:
void Video::playTv(QString fileName, QString channelName)
{
	channel->setCurrentSource(fileName);
	channel->play();

	currentChannel = channelName;
	emit playing(currentChannel);
}
void Video::setVolume(qreal volume)
{
	audio->setVolume(volume);
}
Phonon::VolumeSlider *Video::slider()
{
	return aslider;
}
QString Video::currentMedia()
{
	return channel->currentSource().fileName();
}
qreal Video::volume()
{
	return audio->volume();
}

//Controls:
void Video::controlPlay()
{
	channel->play();
	emit playing(currentChannel);
}
void Video::controlStop()
{
	channel->stop();
	channel->setCurrentSource(QString("udp://"));
	emit stopped();
}
void Video::controlFull()
{
	if (this->isFullScreen() == false) {
		this->enterFullScreen();
	}
	else {
		qApp->setOverrideCursor(Qt::ArrowCursor);
		this->exitFullScreen();
	}
}
void Video::controlMute()
{
	if(audio->isMuted())
		audio->setMuted(false);
	else
		audio->setMuted(true);
}
void Video::controlVUp()
{
	audio->setVolume(audio->volume()+0.1);
	qDebug() << "Volume:" << audio->volume()*100;
}
void Video::controlVDown()
{
	audio->setVolume(audio->volume()-0.1);
	qDebug() << "Volume:" << audio->volume()*100;
}

//Ratio
void Video::ratioOriginal()
{
	this->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
}
void Video::ratio43()
{
	this->setAspectRatio(Phonon::VideoWidget::AspectRatio4_3);
}
void Video::ratio169()
{
	this->setAspectRatio(Phonon::VideoWidget::AspectRatio16_9);
}
void Video::ratioDinamic()
{
	this->setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
}


//Crop
void Video::cropOriginal()
{
	this->setScaleMode(Phonon::VideoWidget::FitInView);
}
void Video::cropFit()
{
	this->setScaleMode(Phonon::VideoWidget::ScaleAndCrop);
}

//Move
void Video::disableMove()
{
	move = false;
}
void Video::enableMove()
{
	move = true;
}
