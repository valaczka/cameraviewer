import QtQuick 2.15
import QtQuick.Controls 2.15
import QtMultimedia 5.15
import CameraViewer 1.0

ApplicationWindow {
	id: mainWindow

	width: 640
	height: 480
	visible: true

	visibility: "FullScreen"

	title: qsTr("Camera Viewer")

	CameraViewer {
		id: cameraViewer
		camera: camera
	}


	Item {
		id : cameraUI

		anchors.fill: parent

		focus: true

		Keys.onReleased: {
			if (event.key === Qt.Key_Back) {
				mainWindow.close()
			}
		}

		Camera {
			id: camera
			captureMode: Camera.CaptureStillImage

			videoRecorder {
				resolution: "2160x1080"
				frameRate: 30
			}

			viewfinder.resolution: "2160x1080"
			viewfinder.minimumFrameRate: 30

			onLockStatusChanged: {
				console.debug("LOCK STATUS", lockStatus)
			}

		}


		VideoOutput {
			id: viewfinder

			anchors.fill: parent

			source: camera
			autoOrientation: true

			Repeater {
				model: camera.focus.focusZones

				Rectangle {
					visible: controlColumn.visible
					border {
						width: 2
						color: status == Camera.FocusAreaFocused ? "green" : "white"
					}
					color: "transparent"

					// Map from the relative, normalized frame coordinates
					property variant mappedRect: viewfinder.mapNormalizedRectToItem(area);

					x: mappedRect.x
					y: mappedRect.y
					width: mappedRect.width
					height: mappedRect.height
				}
			}


			MultiPointTouchArea {
				anchors.fill: parent
				maximumTouchPoints: 1

				touchPoints: [
					TouchPoint {
						id: point
					}
				]

				onPressed: {
					if (touchPoints.length) {
						if (controlColumn.visible) {
							var p = Qt.point(touchPoints[0].x, touchPoints[0].y)
							cameraViewer.lock(viewfinder.mapPointToSourceNormalized(p))
						} else {
							controlColumn.visible = true
						}
					}
				}
			}

			Column {
				id: controlColumn
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				visible: false

				spacing: 5

				onVisibleChanged: if (visible)
									  timer.restart()

				Button {
					text: cameraViewer.focusLock ? "[F]" : "F"
					onClicked: cameraViewer.focusLock = !cameraViewer.focusLock
				}

				Button {
					text: cameraViewer.exposureLock ? "[E]" : "E"
					onClicked: cameraViewer.exposureLock = !cameraViewer.exposureLock
				}

				Button {
					text: cameraViewer.whiteBalanceLock ? "[W]" : "W"
					onClicked: cameraViewer.whiteBalanceLock = !cameraViewer.whiteBalanceLock
				}
			}


			Timer {
				id: timer
				interval: 10000
				repeat: false

				onTriggered: controlColumn.visible = false
			}

		}

	}



	onClosing: {
		close.accepted = true
		Qt.quit()
	}
}


