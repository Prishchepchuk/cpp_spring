#include <cmath>

#include "calculator.hpp"

void Calculator::initialize()
{
	m_field.resize(m_height);

	for (auto& row : m_field)
	{
		row.resize(m_width);
	}
}

void Calculator::calculate(sf::Vector2f center, float width, float height)
{
	auto threads_number = std::thread::hardware_concurrency();

	auto dx = width / m_width;
	auto dy = height / m_height;

	std::vector < std::thread > threads(threads_number);

	auto size = m_height / threads_number; // режем картинку на полосочки для многопоточной обработки
	auto tail = m_height - threads_number * size;

	auto function = [=](std::size_t id)
	{
		auto first_y = size * id;
		auto last_y = first_y + size;

		last_y = m_height;

		for (auto y = first_y; y < last_y; ++y) // проходим по всем точкам поля
		{
			for (auto x = 0U; x < m_width; ++x)
			{
				std::complex < float > z(
					center.x - width / 2.0f + dx * x, // set center of poly
					center.y - height / 2.0f + dy * y
				);

				auto iteration = 0U;
				//auto Rad = (1.0f + std::powf(1.0f + 4.0f * std::abs(m_c), 0.5f)) / 2.0f; //normal circle
				//auto Rad = (3.0f + std::powf(2.0f + 4.0f * std::abs(m_c), 1.5f)) / 2.0f;
				auto Rad = (1.0f + std::powf(2.0f + 2.0f * std::abs(m_c), 4.0f)) / 2.0f; // set Radius 

				for (; iteration < m_max_iterations && std::abs(z) < Rad; ++iteration)
				{
					z = z * z  + m_c; // mega math formula
				}

				m_field[y][x] = iteration;
			}
		}
	};
	
	for (auto i = 0U; i < threads_number; ++i) //simpiest multithreading
	{
		threads[i] = std::thread(function, i);
	}
	

	if (tail > 0U)
	{
		function(threads_number);
	}

	for (auto i = 0U; i < threads_number; ++i)
	{
		threads[i].join();
	}
	
}