#include "GUI.h"
#include "Graphics.h"
#include "Renderer.h"
#include "Log.h"

void GUI :: DefaultStyle :: drawLine(const glm::vec2& a, const glm::vec2& b, eEffectType fx) const
{
	//glBegin(GL_LINES);
	//    glVertex2fv(a.array());
	//    glVertex2fv(b.array());
	//glEnd();
	Renderer::get().drawLine(glm::vec3(a, 0.0f),glm::vec3(b, 0.0f));
}

void GUI :: DefaultStyle :: drawBox(const glm::vec2& pos, const glm::vec2& size, eEffectType fx) const
{
	glm::vec2 v[4] = {
		glm::vec2(pos),
		glm::vec2(pos) + glm::vec2(size.x, 0.0f),
		glm::vec2(pos) + glm::vec2(size),
		glm::vec2(pos) + glm::vec2(0.0f, size.y)
	};
	glBegin(GL_QUADS);
	for(int i=0; i<4; i++) {
		Color c;
		Color c2;
		switch(fx)
		{
			case FX_NONE:
				c = Color(0.5f, 0.5f, 0.5f, 0.2f);
				c2 = Color(0.5f, 0.5f, 0.5f, 0.9f);
				break;
			case FX_HIGHLIGHT: 
				c = Color(0.0f, 0.0f, 0.5f, 0.2f);
				c2 = Color(0.0f, 0.0f, 0.5f, 0.9f);
				break;
			case FX_ACTIVE:
				break;
		};
		//Color c(fx==FX_HIGHLIGHT?0.0f:0.5f, fx==FX_HIGHLIGHT?0.0f:0.5f, 0.5f, (i==0||i==1)?0.2f:0.9f);
		//glColor4fv((i==0||i==1)?c.array():c2.array());
		((i==0||i==1)?c:c2).apply();
		glVertex2fv(glm::value_ptr(v[i]));
	}
	glEnd();
}

void GUI :: DefaultStyle :: drawText(const std::string& text, const glm::vec2& pos, glm::vec2 size, eFontType ftype, eFontAlign falign) const
{
	if(size == glm::vec2(0.0f))
		size = glm::vec2(Renderer::get().width()*1.0f, fontSize()*1.0f);

	FTSimpleLayout layout;
	layout.SetFont(font(ftype));
	layout.SetLineLength(size.x);
	switch(falign)
	{
		case ALIGN_LEFT:
			layout.SetAlignment(FTGL::ALIGN_LEFT);
			break;
		case ALIGN_CENTER:
			layout.SetAlignment(FTGL::ALIGN_CENTER);
			break;
		case ALIGN_RIGHT:
			layout.SetAlignment(FTGL::ALIGN_RIGHT);
			break;
		default:
			return;
	};
	Color::apply(Color()); // apply white color, no trans
	// FTGL overrides layout positioning (bug), so use glRasterPos
	glRasterPos2f(pos.x, pos.y + (size.y - fontSize()) / 2.0f);
	layout.Render(text.c_str());
	glRasterPos2f(0.0f, 0.0f); // reset
}

GUI :: Button :: Button(const std::string& caption, const Freq::Timed<glm::vec2>& pos, const glm::vec2& size, std::function<void(Button*)>& callback, unsigned int flags)
{
	m_flags = flags;
	m_sCaption = caption;
	m_bHighlighted = false;
	m_xCallback = callback;
	position(pos);
	Object::size(size);
}

int GUI::Button :: logic(GUI* gui, unsigned int advance, Style* style, Input* input)
{
	m_bHighlighted = collision(
		glm::vec2(
			input->getMouseX(),
			Renderer::get().height() - input->getMouseY()
		),
		glm::vec2(1.0f)
	);

	if(m_bHighlighted && input->mouseLeftClickf()) {
		gui->focus(this);
		m_xCallback(this);
	}

	return m_bHighlighted?1:0;
}

void GUI::Button :: render(const Style* style) const
{
	glm::vec2 pos = m_pos.get();
	//bool hl = collision(
	//    glm::vec2(
	//        input->getMouseX(),
	//        Renderer::get().height() - input->getMouseY()
	//    ),
	//    glm::vec2(1.0f)
	//);
	//style->drawBox(pos, m_size,
	//   hl? Style::FX_HIGHLIGHT: Style::FX_NONE);
	//style->drawText(m_sCaption, pos, m_size,
	//   hl ? Style::FONT_BOLD : Style::FONT_REGULAR,
	//   Style::ALIGN_CENTER);
	style->drawBox(pos, m_size,
	   m_bHighlighted? Style::FX_HIGHLIGHT: Style::FX_NONE);
	style->drawText(m_sCaption, pos, m_size,
	   m_bHighlighted ? Style::FONT_BOLD : Style::FONT_REGULAR,
	   Style::ALIGN_CENTER);
}

bool GUI::Button :: collision(const glm::vec2& obj, const glm::vec2& size) const
{
	glm::vec2 my_pos = m_pos.get();
	return GUI::collision(my_pos, m_size, obj, size);
}


GUI :: ToggleButton :: ToggleButton(const std::string& caption, const Freq::Timed<glm::vec2>& pos, const glm::vec2& size, std::function<void(ToggleButton*)>& callback, unsigned int flags)
{
	m_flags = flags;
	m_sCaption = caption;
	m_bHighlighted = false;
	m_bActivated = false;
	m_xCallback = callback;
	position(pos);
	Object::size(size);
}

int GUI::ToggleButton :: logic(GUI* gui, unsigned int advance, Style* style, Input* input)
{
	m_bHighlighted = collision(
		glm::vec2(
			input->getMouseX(),
			Renderer::get().height() - input->getMouseY()
		),
		glm::vec2(1.0f)
	);

	if(m_bHighlighted && input->mouseLeftClickf()) {
		//m_bActivated = !m_bActivated;
		//m_xCallback(this);
		gui->focus(this);
		activate(!activated());
	}

	return m_bHighlighted?1:0;
}

void GUI::ToggleButton :: activate(bool b)
{
	if(b == m_bActivated) // don't retrigger
		return;
	m_bActivated = b;
	m_xCallback(this);
}

void GUI::ToggleButton :: render(const Style* style) const
{
	glm::vec2 pos = m_pos.get();
	style->drawBox(pos, m_size,
	   ( m_bHighlighted || m_bActivated ) ? Style::FX_HIGHLIGHT: Style::FX_NONE);
	style->drawText(m_sCaption, pos, m_size,
	   ( m_bHighlighted || m_bActivated ) ? Style::FONT_BOLD : Style::FONT_REGULAR,
	   Style::ALIGN_CENTER);
}

bool GUI::ToggleButton :: collision(const glm::vec2& obj, const glm::vec2& size) const
{
	glm::vec2 my_pos = m_pos.get();
	return GUI::collision(my_pos, m_size, obj, size);
}


GUI::Menu :: Menu(std::list<Option> options, const Freq::Timed<glm::vec2>& pos, const glm::vec2& size, unsigned int flags)
{
	m_flags = flags;
	//for(auto itr = options.begin();
	//   itr != options.end();
	//   ++itr)
	//{
	//   m_Options.push_back(*itr);
	//}
	m_Options = move(options);
	//std::copy(options.begin(), options.end(), m_Options.begin());
	position(pos);
	Object::size(size);
	m_SelectedOption = m_Options.begin();
	m_vDirection = glm::vec2(0.0, -1.0);
}

int GUI::Menu:: logic(GUI* gui, unsigned int advance, Style* style, Input* input)
{
	int affected = 0;
	glm::vec2 my_pos = m_pos.get();
	glm::vec2 pos = my_pos;

	glm::vec2 mouse_pos(
		input->getMouseX(),
		Renderer::get().height() - input->getMouseY()
	);
	glm::vec2 mouse_size(1.0f);
	glm::vec2 list_size(
		m_size.x,
		m_size.y * (float)m_Options.size()
	);
	
	if(GUI::collision(
		pos + (m_vDirection * m_size) * (float)(m_Options.size()-1),
		list_size,
		mouse_pos,
		mouse_size))
	{
		if(input->mouseLeftClickf())
		{
			gui->focus(this);

			for(list<Option>::iterator itr = m_Options.begin();
				itr != m_Options.end();
				++itr)
			{
				if(GUI::collision(pos,
					m_size,
					mouse_pos,
					mouse_size))
				{
					itr->call(this);
					affected++;
					break;
				}
					
				pos += m_vDirection * m_size;
			}
		}
	}
	
	pos = my_pos;


	for(list<Option>::iterator itr = m_Options.begin();
		itr != m_Options.end();
		++itr)
	{
		if(GUI::collision(pos,
			m_size,
			mouse_pos,
			mouse_size))
		{
			m_SelectedOption = itr;
			affected++;
			break;
		}
			
		pos += m_vDirection * m_size;
	}


	return affected;
}

void GUI::Menu:: render(const Style* style) const
{
	glm::vec2 pos = m_pos.get();
	//style->drawBox(pos, glm::vec2(m_vDirection * (m_size * (float)m_Options.size()) ), Style::FX_HIGHLIGHT);

	for(list<Option>::const_iterator itr = m_Options.begin();
		itr != m_Options.end();
		++itr)
	{
		style->drawBox(pos, m_size, itr == m_SelectedOption ? Style::FX_HIGHLIGHT: Style::FX_NONE);
		style->drawText(itr->caption(), pos, m_size,
			itr == m_SelectedOption ? Style::FONT_BOLD : Style::FONT_REGULAR,
			Style::ALIGN_CENTER);
		pos += m_vDirection * m_size;
	}
}

bool GUI::Menu :: collision(const glm::vec2& obj, const glm::vec2& size) const
{
	return GUI::collision(m_pos.get(), glm::vec2(m_size * (float)m_Options.size()), obj, size);
}


int GUI::Dialog::logic(GUI* gui, unsigned int advance, Style* style, Input* input)
{
	return 0;
}

void GUI::Dialog::render(const Style* style) const
{
	glm::vec2 size = Renderer::get().size();
	glm::vec2 v[4] = {
		glm::vec2(0.0f),
		glm::vec2(size.x, 0.0f),
		glm::vec2(size),
		glm::vec2(0.0f, size.y)
	};
	glBegin(GL_QUADS);
	for(int i=0; i<4; i++) {
		Color c(0.0f, 0.0f, 0.0f, 0.5f);
		
		c.apply();
		glVertex2fv(glm::value_ptr(v[i]));
	}
	glEnd();
	//style->drawText();
}


int GUI :: logic(unsigned int advance, Input* input)
{
	if(m_spDialog)
		return m_spDialog->logic(this, advance, m_spStyle.get(), input);

	int affected =  0;
	for(vector<shared_ptr<Object>>::iterator itr = m_Objects.begin();
		itr != m_Objects.end();
		)
	{
		if((*itr)->flags() & GUI::F_REMOVE)
		{
			itr = m_Objects.erase(itr);
			continue;
		}
		if(!(*itr)->hidden())
			affected += (*itr)->logic(this, advance, m_spStyle.get(), input);
		++itr;
	}

	if(!affected && input->mouseAnyf())
	{
		// clear temp items
		for(vector<shared_ptr<Object>>::iterator itr = m_Objects.begin();
			itr != m_Objects.end();
			)
		{
			if((*itr)->flags() & GUI::F_AUTOHIDE)
				(*itr)->hide(true);
			else if((*itr)->flags() & GUI::F_AUTODEACTIVATE)
				(*itr)->activate(false);
			else if((*itr)->flags() & GUI::F_AUTOREMOVE || (*itr)->flags() & GUI::F_REMOVE)
			{
				itr = m_Objects.erase(itr);
				continue;
			}
			++itr;
		}
	}

	return affected;
}


void GUI :: render() const
{
	for(vector<shared_ptr<Object>>::const_iterator itr = m_Objects.cbegin();
		itr != m_Objects.cend();
		++itr)
	{
		if(!((*itr)->hidden() && (*itr)->position().hasElapsed()))
			(*itr)->render(m_spStyle.get());
	}

	if(m_spDialog)
	{
		m_spDialog->render(m_spStyle.get());
	
		style()->drawText(string("This is a status message."), glm::vec2(0.0f), glm::vec2(0.0f),
		   Style::FONT_REGULAR, Style::ALIGN_CENTER);
	}
}

bool GUI :: collision(
	const glm::vec2& a_pos,
	const glm::vec2& a_size,
	const glm::vec2& b_pos,
	const glm::vec2& b_size)
{
	if ((a_pos.x > b_pos.x + b_size.x - 1.0f)||
		(a_pos.y > b_pos.y + b_size.y - 1.0f)||
		(b_pos.x > a_pos.x + a_size.x - 1.0f)||
		(b_pos.y > a_pos.y + a_size.y - 1.0f))
	{
		return false;
	}
	return true;
}

void GUI :: focus(Object* obj)
{
	for(vector<shared_ptr<Object>>::iterator itr = m_Objects.begin();
		itr != m_Objects.end();
		)
	{
		if(itr->get() != obj)
		{
			if((*itr)->flags() & GUI::F_AUTOHIDE)
				(*itr)->hide(true);
			else if((*itr)->flags() & GUI::F_AUTODEACTIVATE)
				(*itr)->activate(false);
			else if((*itr)->flags() & GUI::F_AUTOREMOVE || (*itr)->flags() & GUI::F_AUTOREMOVE)
			{
				itr = m_Objects.erase(itr);
				continue;
			}
		}
		++itr;
	}
}

