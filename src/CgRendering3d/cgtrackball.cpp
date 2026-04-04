
#include "cgtrackball.h"

#include <cmath>


CgTrackball::CgTrackball()
	: center(0.0),
	  vNow(0.0, 0.0, 0.0),
	  vDown(0.0, 0.0, 0.0),
	  qNow(1.0, 0.0, 0.0, 0.0),
	  qDown(1.0, 0.0, 0.0, 0.0)
{
	init();
}


/* Establish reasonable initial values for controller. */
void CgTrackball::init(const glm::mat4& m)
{
	mNow = mDown = m;
	qDown = glm::quat_cast(mNow);
	radius = 1.0;
	dragging = false;
}

/* Set the center and size of the controller. */
void CgTrackball::place(const glm::vec3& nCenter, float nRadius)
{
	center = nCenter;
	radius = nRadius;
}

/* Incorporate new mouse position. */
void CgTrackball::Ball_Mouse(glm::vec3 nvNow)
{
	vNow = nvNow;
}

/* Using vDown, vNow, dragging, compute rotation etc. */
void CgTrackball::Ball_Update()
{
	vFrom = MouseOnSphere(vDown, center, radius);
	vTo = MouseOnSphere(vNow, center, radius);

	if (dragging)
	{
		// construct unit quaternion from two points on sphere
		glm::vec3 perp = glm::cross(vFrom, vTo);
		float angle = glm::dot(vFrom, vTo);
		qDrag = glm::quat(angle, perp);
		qNow = qDrag * qDown;
	}
	Qt_ToBallPoints(qDown, &vrFrom, &vrTo);

	mNow = glm::mat4_cast(qNow);
}

/* Return rotation matrix defined by controller use. */
const glm::mat4& CgTrackball::getRotationMatrix() const
{
	return mNow;
}


/* Begin drag sequence. */
void CgTrackball::beginDrag()
{
	dragging = true;
	vDown = vNow;
	vTo = vFrom;
}

/* Stop drag sequence. */
void CgTrackball::endDrag()
{
	dragging = false;
	qDown = qNow;
	mDown = mNow;
}

/* Convert window coordinates to sphere coordinates. */
glm::vec3 CgTrackball::MouseOnSphere(glm::vec3 mouse, glm::vec3 ballCenter, float ballRadius)
{
	glm::vec3 ballMouse;

	ballMouse.x = (mouse.x - ballCenter.x) / ballRadius;
	ballMouse.y = (mouse.y - ballCenter.y) / ballRadius;
	const float mag = ballMouse.x * ballMouse.x + ballMouse.y * ballMouse.y;
	if (mag > 1.0)
	{
		const float scale = 1.0f / std::sqrt(mag);
		ballMouse.x *= scale;
		ballMouse.y *= scale;
		ballMouse.z = 0.0;
	}
	else
	{
		ballMouse.z = std::sqrt(1 - mag);
	}
	return (ballMouse);
}


/* Convert a unit quaternion to two points on unit sphere */
void CgTrackball::Qt_ToBallPoints(glm::quat q, glm::vec3* arcFrom, glm::vec3* arcTo)
{
	double s = sqrt(q.x * q.x + q.y * q.y);
	if (s == 0.0)
	{
		*arcFrom = glm::vec3(0.0, 1.0, 0.0);
	}
	else
	{
		*arcFrom = glm::vec3(-q.y / s, q.x / s, 0.0);
	}

	(*arcTo) = q * (*arcFrom);

	//arcTo->x = q.w*arcFrom->x - q.z*arcFrom->y;
	//arcTo->y = q.w*arcFrom->y + q.z*arcFrom->x;
	//arcTo->z = q.x*arcFrom->y - q.y*arcFrom->x;
	if (q.w < 0.0) *arcFrom = glm::vec3(-arcFrom->x, -arcFrom->y, 0.0);
}


/* Force sphere point to be perpendicular to axis. */
// ReSharper disable once CppDFAUnreachableFunctionCall
glm::vec3 CgTrackball::ConstrainToAxis(const glm::vec3 loose, const glm::vec3 axis)
{
	glm::vec3 onPlane = loose - axis * glm::dot(axis, loose);

	if (glm::length(onPlane) > 0.0)
	{
		if (onPlane.z < 0.0) onPlane = -onPlane;
		return glm::normalize(onPlane);
	} /* else drop through */
	if (axis.z == 1.0)
	{
		onPlane = glm::vec3(1.0, 0.0, 0.0);
	}
	else
	{
		onPlane = glm::normalize(glm::vec3(-axis.y, axis.x, 0.0));
	}
	return (onPlane);
}

/* Find the index of nearest arc of axis set. */
unsigned int CgTrackball::NearestConstraintAxis(const glm::vec3 loose, const std::vector<glm::vec3>& axes)
{
	float max = -1;
	unsigned int nearest = 0;
	for (unsigned int i = 0; i < axes.size(); i++)
	{
		glm::vec3 onPlane = ConstrainToAxis(loose, axes[i]);
		const float dot = glm::dot(onPlane, loose);
		if (dot > max)
		{
			max = dot;
			nearest = i;
		}
	}
	return (nearest);
}

