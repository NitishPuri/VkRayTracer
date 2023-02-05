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

void Renderer::Render(const Scene& scene, const Camera& cam)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &cam;


	for (uint32_t y = 0; y < m_FinalImage->GetHeight() ; y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			auto color = PerPixel(x, y);
			auto i = x + y * m_FinalImage->GetWidth();
			m_ImageData[i] = Utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

	auto hitPayload = TraceRay(ray);

	if (hitPayload.HitDistance < 0) {
		return glm::vec4(0, 0, 0, 1);
	}

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
	float lightIntensity = glm::max(glm::dot(hitPayload.WorldNormal, -lightDir), 0.0f);

	const Sphere& closestSphere = m_ActiveScene->Spheres[hitPayload.ObjectIndex];

	glm::vec3 sphereColor = closestSphere.Albedo;
	sphereColor *= lightIntensity;

	//color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

	return glm::vec4(sphereColor, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	int closestSphereIdx = -1;
	float hitDistance = std::numeric_limits<float>::max();
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); ++i ) {
		const auto& sphere = m_ActiveScene->Spheres[i];
		glm::vec3 origin = ray.Origin - sphere.Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

		float discrminant = (b * b) - (4.0f * a * c);

		if (discrminant < 0.0f) {
			continue;
		}

		float t = (-b - glm::sqrt(discrminant)) / (2.0f * a);
		if (t < hitDistance) {
			closestSphereIdx = i;
			hitDistance = t;
		}
	}

	if (closestSphereIdx < 0) {
		return Miss(ray);
	}

	return ClosestHit(ray, hitDistance, closestSphereIdx);

}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	const auto& closestSphere = m_ActiveScene->Spheres[objectIndex];

	HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	glm::vec3 origin = ray.Origin - closestSphere.Position;
	payload.WorldPosition = origin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);
	payload.WorldPosition += closestSphere.Position;

	return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
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
