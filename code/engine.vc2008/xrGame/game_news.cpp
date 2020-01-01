///////////////////////////////////////////////////////////////
// game_news.cpp
// реестр новостей: новости симуляции + сюжетные
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_news.h"
#include "object_broker.h"

#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "game_graph.h"

#include "../xrEngine/date_time.h"
#include "xrServer_Objects_ALife_Monsters.h"

#include "specific_character.h"

GAME_NEWS_DATA::GAME_NEWS_DATA()
{
	m_type			= eNews;
	show_time		= DEFAULT_NEWS_SHOW_TIME;
}

void GAME_NEWS_DATA::save (IWriter& stream)
{
	save_data(m_type,		stream);
	save_data(news_caption,	stream);
	save_data(news_text,	stream);
	save_data(receive_time, stream);
	save_data(texture_name,	stream);
}

void GAME_NEWS_DATA::load (IReader& stream)
{
	load_data(m_type,		stream);
	load_data(news_caption,	stream);
	load_data(news_text,	stream);
	load_data(receive_time, stream);
	load_data(texture_name,	stream);
}