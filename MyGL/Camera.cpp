#include "Camera.h"

MyGL::Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
	: position(position), world_up(up), yaw(yaw), pitch(pitch)
{
	update_camera_vectors();
}

void MyGL::Camera::update_camera_vectors()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	right = glm::normalize(glm::cross(front, world_up));
	up = glm::normalize(glm::cross(right, front));
}

glm::mat4 MyGL::Camera::get_model_matrix() const
{
	return glm::translate(glm::mat4(1.0f), -position);
}

glm::mat4 MyGL::Camera::get_view_matrix() const
{
	return glm::lookAt(position, position + front, up);
}

glm::mat4 MyGL::Camera::get_projection_matrix(float aspect_ratio) const
{
	return glm::perspective(glm::radians(zoom), aspect_ratio, 0.1f, 100.0f);
}

void MyGL::Camera::set_position(glm::vec3 position)
{
	this->position = position;
}

void MyGL::Camera::look_at(glm::vec3 target)
{
	front = glm::normalize(target - position);
	yaw = glm::degrees(atan2(front.z, front.x));
	pitch = glm::degrees(asin(front.y));

	right = glm::normalize(glm::cross(front, world_up));
	up = glm::normalize(glm::cross(right, front));
}

void MyGL::Camera::on_keyboard(KeyboardMoveDirection direction, float delta_time)
{
	float velocity = movement_speed * delta_time;

	switch (direction)
	{
	case KeyboardMoveDirection::FORWARD:
		position += front * velocity;
		break;
	case KeyboardMoveDirection::BACKWARD:
		position -= front * velocity;
		break;
	case KeyboardMoveDirection::LEFT:
		position -= right * velocity;
		break;
	case KeyboardMoveDirection::RIGHT:
		position += right * velocity;
		break;
	}
}

void MyGL::Camera::on_mouse_movement(float x_offset, float y_offset, bool constrain_pitch)
{
	x_offset *= mouse_sensitivity_x;
	y_offset *= mouse_sensitivity_y;

	yaw += x_offset;
	pitch += y_offset;

	if (constrain_pitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	update_camera_vectors();
}

void MyGL::Camera::on_mouse_scroll(MouseZoomDirection zoom_direction)
{
	if (zoom_direction == MouseZoomDirection::IN)
		zoom -= 0.5f;
	if (zoom_direction == MouseZoomDirection::OUT)
		zoom += 0.5f;

	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 45.0f)
		zoom = 45.0f;
}

void MyGL::Camera::on_lstick(float x, float y, float delta_time)
{
	float velocity = movement_speed * delta_time;

	position += front * y * velocity;
	position += right * x * velocity;
}

void MyGL::Camera::on_rstick(float x, float y, float delta_time, bool constrain_pitch)
{
	x *= controller_sensitivity_x;
	y *= controller_sensitivity_y;

	yaw += x;
	pitch += y;

	if (constrain_pitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	update_camera_vectors();
}
