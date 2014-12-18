struct BowlingBall{

	Model m;
	glm::vec3 position;
	float rotationAngle;

	void setPosition(glm::vec3 pos)
	{
		position = pos;
	}

	void setRotationAngle(float angle)
	{
		rotationAngle = angle;
	}

	void setNewPosition(glm::vec3 newPos, float newAngle)
	{
		position = newPos;
		rotationAngle = newAngle;
		
		m.model = glm::translate(glm::mat4(1.0f), newPos);
  		m.model = glm::rotate(m.model, newAngle, glm::vec3(0.0f, 1.0f, 0.0f));
  		m.model = glm::scale(m.model, glm::vec3(0.5f, 0.5f, 0.5f));
	}

	void resetBallPosition()
	{
		m.model = glm::translate(glm::mat4(1.0f), position);
  		m.model = glm::rotate(m.model, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
  		m.model = glm::scale(m.model, glm::vec3(0.5f, 0.5f, 0.5f));
	}

	void repositionBall(glm::vec3 newPos, float newAngle)
	{
		m.model = glm::translate(glm::mat4(1.0f), newPos);
  		m.model = glm::rotate(m.model, newAngle, glm::vec3(0.0f, 1.0f, 0.0f));
  		m.model = glm::scale(m.model, glm::vec3(0.5f, 0.5f, 0.5f));
	}

};