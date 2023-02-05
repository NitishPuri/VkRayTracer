#include "Renderer.h"
#include "Walnut/Random.h"

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color) {
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | (r);
		return result;
	}
}

void Renderer::Render(const Camera& cam)
{
	Ray ray;
	ray.Origin = cam.GetPosition();

	for (uint32_t y = 0; y < m_FinalImage->GetHeight() ; y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {

			ray.Direction = cam.GetRayDirections()[x + y * m_FinalImage->GetWidth()];			
			auto color = TraceRay(ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			auto i = x + y * m_FinalImage->GetWidth();
			m_ImageData[i] = Utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Ray& ray)
{
	float radius = 0.5f;

	float a = glm::dot(ray.Direction, ray.Direction);
	float b = 2.0f * glm::dot(ray.Origin, ray.Direction);
	float c = glm::dot(ray.Origin, ray.Origin) - radius * radius;

	float discrminant = (b * b) - (4.0f * a * c);

	if (discrminant < 0.0f) {
		return glm::vec4(0, 0, 0, 1);
	}

	float t = (-b - glm::sqrt(discrminant)) / (2.0f * a);
	glm::vec3 hitPoint = ray.Origin + ray.Direction * t;
	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
	float lightIntensity = glm::max(glm::dot(normal, -lightDir), 0.0f);

	glm::vec3 sphereColor(1, 0, 1);
	sphereColor *= lightIntensity;

	return glm::vec4(sphereColor, 1);
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
