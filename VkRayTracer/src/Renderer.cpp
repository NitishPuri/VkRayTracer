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
	return Walnut::Random::UInt();
	//return 0xffff00ff;
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
