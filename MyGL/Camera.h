#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace MyGL
{
	// Default camera values
	const glm::vec3 POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
	const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 2.5f;
	const float MOUSE_SENSITIVITY_X = 0.1f;
	const float MOUSE_SENSITIVITY_Y = 0.1f;
	const float CONTROLLER_SENSITIVITY_X = 1.0f;
	const float CONTROLLER_SENSITIVITY_Y = 1.0f;
	const float ZOOM = 45.0f;

	class Camera
	{
	public:
		Camera(glm::vec3 position = POSITION,
			   glm::vec3 up = UP,
			   float yaw = YAW,
			   float pitch = PITCH);

		glm::mat4 get_view_matrix() const;
		glm::mat4 get_model_matrix() const;
		glm::mat4 get_projection_matrix(float aspect_ratio) const;

		void set_position(glm::vec3 position); // Set position
		void look_at(glm::vec3 target);		   // Look at specific point

		// Keyboard and mouse input
		enum class KeyboardMoveDirection
		{
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT
		};
		enum class MouseZoomDirection
		{
			IN,
			OUT
		};
		void on_keyboard(KeyboardMoveDirection direction, float delta_time);
		void on_mouse_movement(float x_offset, float y_offset,
							   bool constrain_pitch = true);
		void on_mouse_scroll(MouseZoomDirection zoom_direction);

		// Gamepad input
		void on_lstick(float x, float y, float delta_time);
		void on_rstick(float x, float y, float delta_time,
					   bool constrain_pitch = true);

	private:
		glm::vec3 position;

		glm::vec3 front;
		glm::vec3 up;
		glm::vec3 right;

		glm::vec3 world_up;

		float yaw;
		float pitch;

		float movement_speed = SPEED;
		float mouse_sensitivity_x = MOUSE_SENSITIVITY_X;
		float mouse_sensitivity_y = MOUSE_SENSITIVITY_Y;
		float controller_sensitivity_x = CONTROLLER_SENSITIVITY_X;
		float controller_sensitivity_y = CONTROLLER_SENSITIVITY_Y;
		float zoom = ZOOM;

		void update_camera_vectors();
	};
}