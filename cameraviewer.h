/*
 * ---- Call of Suli ----
 *
 * cameraviewer.h
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

#ifndef CAMERAVIEWER_H
#define CAMERAVIEWER_H

#include <QObject>
#include <QCamera>
#include <QVariant>

class CameraViewer : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QObject* camera READ camera WRITE setCamera NOTIFY cameraChanged)
	Q_PROPERTY(bool focusLock READ focusLock WRITE setFocusLock NOTIFY focusLockChanged)
	Q_PROPERTY(bool exposureLock READ exposureLock WRITE setExposureLock NOTIFY exposureLockChanged)
	Q_PROPERTY(bool whiteBalanceLock READ whiteBalanceLock WRITE setWhiteBalanceLock NOTIFY whiteBalanceLockChanged)

public:
	explicit CameraViewer(QObject *parent = nullptr);

	QObject* camera() const;
	void setCamera(QObject *newCamera);

	bool focusLock() const;
	void setFocusLock(bool newFocusLock);

	bool exposureLock() const;
	void setExposureLock(bool newExposureLock);

	bool whiteBalanceLock() const;
	void setWhiteBalanceLock(bool newWhiteBalanceLock);

public slots:
	static void keepScreenOn();
	void lock(const QPointF &point);

signals:
	void cameraChanged();
	void focusLockChanged();
	void exposureLockChanged();
	void whiteBalanceLockChanged();

private:
	void setRealCamera(QCamera *camera);

	QObject *m_camera;
	QCamera *m_realCamera;
	bool m_focusLock;
	bool m_exposureLock;
	bool m_whiteBalanceLock;
};

#endif // CAMERAVIEWER_H
