#ifndef _GUI_H
#define _GUI_H

#include <vector>
#include <memory>
#include <initializer_list>
#include <functional>
using namespace std;

#include "Renderer.h"
#include "Input.h"
#include "math/common.h"
#include "IFallible.h"
#include "Freq.h"

#include <FTGL/ftgl.h>
#include "Util.h"

class GUI : public IFallible
{
	public:

		enum {
			F_HIDDEN = BIT(0),
			F_AUTODEACTIVATE = BIT(1), // deactivates on GUI focus loss
			F_AUTOHIDE = BIT(2), // autohides on GUI focus loss
			F_AUTOREMOVE = BIT(3),
			F_REMOVE = BIT(4),
			F_NEEDS_CONFIRM = BIT(5)
		};

		class Style
		{
			public:

				enum eFontType
				{
					FONT_REGULAR = 0,
					FONT_BOLD,
					FONT_ITALTIC
				};

				enum eFontAlign
				{
					ALIGN_LEFT = 0,
					ALIGN_CENTER,
					ALIGN_RIGHT
				};

				enum eEffectType
				{
					FX_NONE = 0,
					FX_HIGHLIGHT,
					FX_ACTIVE,
				};

				explicit Style(std::shared_ptr<FTGLPixmapFont>& font):
					m_spFont(font){}
				virtual ~Style() {}
				virtual void drawLine(const glm::vec2& a, const glm::vec2& b, eEffectType fx = FX_NONE) const {}
				virtual void drawBox(const glm::vec2& pos, const glm::vec2& size, eEffectType fx = FX_NONE) const {}
				virtual void drawText(const std::string& text, const glm::vec2& pos, glm::vec2 size, eFontType ftype = FONT_REGULAR, eFontAlign falign = ALIGN_LEFT) const {}
				virtual FTGLPixmapFont* font(eFontType type = FONT_REGULAR) const {return const_cast<FTGLPixmapFont*>(m_spFont.get()); }
				virtual float fontSize() const { return (float)m_spFont->FaceSize(); }

			private:
				std::shared_ptr<FTGLPixmapFont>& m_spFont;
		};

		class DefaultStyle : public Style
		{
			public:
				explicit DefaultStyle(
					std::shared_ptr<FTGLPixmapFont>& font,
					std::shared_ptr<FTGLPixmapFont>& boldfont):
					Style(font),
					m_spBoldFont(boldfont)
				{}
				virtual ~DefaultStyle() {}
				virtual void drawLine(const glm::vec2& a, const glm::vec2& b, eEffectType fx = FX_NONE) const;
				virtual void drawBox(const glm::vec2& pos, const glm::vec2& size, eEffectType fx = FX_NONE) const;
				virtual void drawText(const std::string& text, const glm::vec2& pos, glm::vec2 size, eFontType ftype = FONT_REGULAR, eFontAlign falign = ALIGN_LEFT) const;
				virtual FTGLPixmapFont* font(Style::eFontType type = FONT_REGULAR) const {
					switch(type) {
						case Style::FONT_BOLD:
							return const_cast<FTGLPixmapFont*>(m_spBoldFont.get());
					}
					return Style::font();
				}

			private:
				std::shared_ptr<FTGLPixmapFont>& m_spBoldFont;
		};

		class Object
		{
			public:
				Object():
					m_flags(0) {}
				virtual ~Object() {}

				virtual int logic(GUI* gui, unsigned int advance, Style* style, Input* input) = 0;
				virtual void render(const Style* style) const = 0;

				const Freq::Timed<glm::vec2>& position() const { return m_pos; }
				Freq::Timed<glm::vec2>& position() { return m_pos; }
				glm::vec2 size() const { return m_size; }
				void position(const glm::vec2& v) {
					m_pos.clear(v);
				}
				void position(const Freq::Timed<glm::vec2>& pos) {
					m_pos = pos;
				}
				void size(const glm::vec2& v) {
					m_size = v;
				}
				virtual bool collision() const {return false;}

				virtual void unfocus() {}
				
				unsigned int flags() { return m_flags; }
				void flags(unsigned int f) { m_flags = f; }

				void hide(bool b) {
					if(b)
						m_flags |= GUI::F_HIDDEN;
					else
						m_flags &= ~GUI::F_HIDDEN;
				}
				bool hidden() {
					return m_flags & GUI::F_HIDDEN;
				}

				virtual bool activated() const { return false; }
				virtual void activate(bool b) {}

			protected:

				Freq::Timed<glm::vec2> m_pos;
				glm::vec2 m_size;
				unsigned int m_flags;
		};

		class Button : public Object
		{
			public:

				Button(const std::string& caption, const Freq::Timed<glm::vec2>& pos, const glm::vec2& size, std::function<void(Button*)>& callback, unsigned int flags = 0);
				virtual ~Button() {}

				virtual int logic(GUI* gui, unsigned int advance, Style* style, Input* input);
				virtual void render(const Style* style) const;
				virtual bool collision(const glm::vec2& obj, const glm::vec2& size) const;

				std::string caption() const { return m_sCaption; }
				
			private:
				
				std::string m_sCaption;
				std::function<void(Button*)> m_xCallback;
				bool m_bHighlighted;
		};

		class ToggleButton : public Object
		{
			public:
				ToggleButton(const std::string& caption, const Freq::Timed<glm::vec2>& pos, const glm::vec2& size, std::function<void(ToggleButton*)>& callback, unsigned int flags = 0);
				virtual ~ToggleButton() {}

				virtual int logic(GUI* gui, unsigned int advance, Style* style, Input* input);
				virtual void render(const Style* style) const;
				virtual bool collision(const glm::vec2& obj, const glm::vec2& size) const;

				std::string caption() const { return m_sCaption; }

				virtual bool activated() const { return m_bActivated; }
				virtual void activate(bool b);

			private:
				std::string m_sCaption;
				std::function<void(ToggleButton*)> m_xCallback;
				bool m_bHighlighted;
				bool m_bActivated;
		};

		class Menu : public Object {
			public:
				struct Option {
					Option(std::string caption, std::function<void(Menu*, Option*)> cb):
						m_caption(caption),
						m_callback(cb) {}
					std::string m_caption;
					std::function<void(Menu*, Option*)> m_callback;

					std::string caption() const { return m_caption; }
					void call(Menu* m) {
						m_callback(m, this);
					}
				};

				//Menu(std::initializer_list<Option> options);
				Menu(std::list<Option> options, const Freq::Timed<glm::vec2>& pos, const glm::vec2& size, unsigned int flags = 0);
				virtual ~Menu() {}

				virtual int logic(GUI* gui, unsigned int advance, Style* style, Input* input);
				virtual void render(const Style* style) const;
				virtual bool collision(const glm::vec2& obj, const glm::vec2& size) const;

			private:
				std::list<Option> m_Options;
				std::list<Option>::iterator m_SelectedOption;
				glm::vec2 m_vDirection;
		};

		class Dialog {
			public:
				Dialog(const std::string& title,
					const std::string& caption,
					std::function<void(Dialog*)> callback,
					std::string yes="Yes",
					std::string no="No"):

					m_sTitle(title),
					m_sCaption(caption),
					m_xCallback(callback),
					m_sYes(yes),
					m_sNo(no)
				{}

				virtual ~Dialog() {}
				virtual int logic(GUI* gui, unsigned int advance, Style* style, Input* input);
				virtual void render(const Style* style) const;
			private:
				std::string m_sTitle;
				std::string m_sCaption;
				function<void(Dialog*)> m_xCallback;
				std::string m_sYes;
				std::string m_sNo;
				// opacity fader?
		};
		
		explicit GUI(shared_ptr<FTGLPixmapFont>& font){
			m_spStyle.reset(new Style(font)); // no theme
		}
		explicit GUI(Style* style) {
			m_spStyle.reset(style);
		}
		virtual ~GUI() {}

		int logic(
			unsigned int advance,
			Input* input
		);
		void render() const;

		bool add(Object* obj) {
			m_Objects.push_back(std::shared_ptr<Object>(obj));
			return true;
		}
		bool add(shared_ptr<Object> obj) {
			m_Objects.push_back(obj);
			return true;
		}
		bool remove(Object* obj) {
			std::remove_if(m_Objects.begin(), m_Objects.end(),
				[obj](shared_ptr<Object> sp){
					return sp.get() == obj;
				}
			);
			return true;
		}

		void clear() {
			m_Objects.clear();
		}

		const Style* style() const { return m_spStyle.get(); }
		void style(Style* style) {
			m_spStyle.reset(style);
		}
		
		static bool collision(
			const glm::vec2& a_pos,
			const glm::vec2& a_size,
			const glm::vec2& b_pos,
			const glm::vec2& b_size);

		// performs object's "auto-" actions for all objects except the one specified
		void focus(Object* obj);

		void dialog(Dialog* dialog) { m_spDialog.reset(dialog); }
		Dialog* dialog() { return m_spDialog.get(); }
		const Dialog* dialog() const { return m_spDialog.get(); }

		bool blocking() const { return m_spDialog.get()!=NULL; }

	private:

		std::vector<std::shared_ptr<Object>> m_Objects;
		std::unique_ptr<Style> m_spStyle;
		std::shared_ptr<Dialog> m_spDialog;
};

#endif

