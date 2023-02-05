#include "Renderer.h"
#include "Walnut/Random.h"

void Renderer::Render()
{
	for (uint32_t y = 0; y < m_FinalImage->GetHeight() ; y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {

			glm::vec2 coord{ (x / (float)m_FinalImage->GetWidth()), (y / (float)m_FinalImage->GetHeight()) };

			auto i = x + y * m_FinalImage->GetWidth();
			m_ImageData[i] = PerPixel(coord);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	coord = (coord * 2.f) - 1.f;
	uint32_t r = (uint32_t)(coord.x * 255.0f);
	uint32_t g = (uint32_t)(coord.y * 255.0f);

	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
	glm::vec3 rayDirection{ coord.x, coord.y, -1.0f };
	float radius = 0.5f;

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	float discrminant = (b * b) - (4 * a * c);
	if (discrminant >= 0.0f) {
		return 0xffff00ff;
	}

	return 0xff000000;
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage) {
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;
		m_FinalImage->Resize(width, height);
	}
	else {
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}
