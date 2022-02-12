/*
 * ---- Call of Suli ----
 *
 * cameraviewer.cpp
 *
 * Created on: 2022. 02. 12.
 *     Author: Valaczka János Pál <valaczka.janos@piarista.hu>
 *
 * CameraViewer
 *
 *  This file is part of Call of Suli.
 *
 *  Call of Suli is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "cameraviewer.h"

#include <QtAndroidExtras>
#include <QCamera>
#include <QCameraInfo>
#include <QDebug>

CameraViewer::CameraViewer(QObject *parent)
	: QObject{parent}
	, m_camera(nullptr)
	, m_realCamera(nullptr)
	, m_focusLock(false)
	, m_exposureLock(false)
	, m_whiteBalanceLock(false)
{
	const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
	for (const QCameraInfo &cameraInfo : cameras) {
		qDebug() << "+" << cameraInfo.deviceName();
	}
}


/**
 * @brief CameraViewer::keepScreenOn
 */

void CameraViewer::keepScreenOn()
{
	QtAndroid::runOnAndroidThread([]{
		QAndroidJniObject activity = QtAndroid::androidActivity();
		if (activity.isValid()) {
			QAndroidJniObject window =
					activity.callObjectMethod("getWindow", "()Landroid/view/Window;");

			if (window.isValid()) {
				const int FLAG_KEEP_SCREEN_ON = 128;
				window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
				qDebug() << "ON";
			}
		}
		QAndroidJniEnvironment env;
		if (env->ExceptionCheck()) {
			env->ExceptionClear();
		}
	});
}



/**
 * @brief CameraViewer::lock
 */

void CameraViewer::lock(const QPointF &point)
{
	if (!m_realCamera)
		return;

	QCamera::LockTypes locks = QCamera::LockTypes();

	if (!m_focusLock)
		locks |= QCamera::LockFocus;

	if (!m_exposureLock)
		locks |= QCamera::LockExposure;

	if (!m_whiteBalanceLock)
		locks |= QCamera::LockWhiteBalance;

	qDebug() << "LOCK START" << m_realCamera << point << locks;

	if (!locks)
		return;

	QCameraFocus *focus = m_realCamera->focus();
	focus->setCustomFocusPoint(point);
	focus->setFocusPointMode(QCameraFocus::FocusPointCustom);
	focus->setFocusMode(QCameraFocus::AutoFocus);

	m_realCamera->searchAndLock(locks);
}


/**
 * @brief CameraViewer::camera
 * @return
 */

QObject *CameraViewer::camera() const
{
	return m_camera;
}

/**
 * @brief CameraViewer::setCamera
 * @param newCamera
 */

void CameraViewer::setCamera(QObject *newCamera)
{
	if (m_camera == newCamera)
		return;

	m_camera = newCamera;
	emit cameraChanged();

	if (m_camera)
		setRealCamera(qvariant_cast<QCamera*>(m_camera->property("mediaObject")));
	else
		setRealCamera(nullptr);
}



/**
 * @brief CameraViewer::setRealCamera
 * @param camera
 */

void CameraViewer::setRealCamera(QCamera *camera)
{
	if (camera == m_realCamera)
		return;

	m_realCamera = camera;

	qDebug() << "CAMERA" << m_realCamera << m_realCamera->supportedLocks() << m_realCamera->requestedLocks();

	connect(m_realCamera, QOverload<QCamera::LockType, QCamera::LockStatus, QCamera::LockChangeReason>::of(&QCamera::lockStatusChanged),
			[=](QCamera::LockType lock, QCamera::LockStatus status, QCamera::LockChangeReason reason){
		qDebug() << "LSC" << lock << status << reason;
	});

	connect(m_realCamera->exposure(), &QCameraExposure::apertureChanged, this, [=](qreal c) {
		qDebug() << "A" << c << m_realCamera->exposure()->aperture();
	});

	connect(m_realCamera->exposure(), &QCameraExposure::shutterSpeedChanged, this, [=](qreal c) {
		qDebug() << "S" << c << m_realCamera->exposure()->shutterSpeed();
	});
}

bool CameraViewer::whiteBalanceLock() const
{
	return m_whiteBalanceLock;
}

void CameraViewer::setWhiteBalanceLock(bool newWhiteBalanceLock)
{
	if (m_whiteBalanceLock == newWhiteBalanceLock)
		return;
	m_whiteBalanceLock = newWhiteBalanceLock;
	emit whiteBalanceLockChanged();
}

bool CameraViewer::exposureLock() const
{
	return m_exposureLock;
}

void CameraViewer::setExposureLock(bool newExposureLock)
{
	if (m_exposureLock == newExposureLock)
		return;
	m_exposureLock = newExposureLock;
	emit exposureLockChanged();
}

bool CameraViewer::focusLock() const
{
	return m_focusLock;
}

void CameraViewer::setFocusLock(bool newFocusLock)
{
	if (m_focusLock == newFocusLock)
		return;
	m_focusLock = newFocusLock;
	emit focusLockChanged();
}
