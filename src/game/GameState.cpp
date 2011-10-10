#include "GameState.h"
#include "../Scene.h"
#include "../RenderBuffer.h"
#include "../Entity.h"
#include "../Light.h"
#include "../Sound.h"
#include "../ParticleSystem.h"

GameState::GameState(Engine* engine, TransitionInfo* trans)
{
	nullify();
	m_pEngine = engine;

	m_pScene = new Scene("data/environment/Apartment/apartment_scene.obj", Scene::F_PHYSICS);
	//m_pScene = new Scene("testmap.fml", Scene::F_PHYSICS);
	//m_pScene = new Scene("data/maps/office/ctf_office.obj");
	
	if(!m_pScene->hasError())
		Log::get().write("Scene initialized.");
	else
	{
		Log::get().error(m_pScene->getError());
		setError("Failed to initiate game state.");
	}

	//Node* n = m_pScene->loadAI("data/assets/tree1.obj");
	//n->move(glm::vec3(-20.0, 0.0, 0.0));
	
	//n = m_pScene->loadAI("data/environment/officebuilding.obj");
	//n->move(glm::vec3(20.0, 0.0, 0.0));
	
	//for(int i=0; i<10; i++)
	//    for(int j=0; j<10; j++)
	//    {
	//        n = m_pScene->loadEntity("data/assets/grass.obj", glm::vec3(1.0*i, -0.6, -1.0*j), Entity::F_PASSABLE);
	//        *n->matrix() *= glm::scale(glm::mat4(), glm::vec3(2.0f));
	//    }
	
	
	//n = m_pScene->loadAI("data/assets/grassStrands.obj");
	//Matrix::translate(*n->matrix(), glm::vec3(5.0, 0.0, 0.0));
	
	m_pScene->add(m_pPlayer = new Actor(engine->input()));

	// Viewmodel Weapon (UMP45)
	//Entity* e = m_pScene->loadEntity("data/entities/ump45/ump45.obj",
	//    glm::vec3(),
	//    Entity::F_PASSABLE | Entity::F_ONTOP,
	//    Node::defaultFlags(),
	//    m_pPlayer
	//);

	//glm::vec3 gunpos = glm::vec3(0.15f,-0.2f,-0.4f);//glock
	glm::vec3 gunpos = glm::vec3(0.15f,-0.3f,-0.5f);
	//glm::vec3 gunpos = glm::vec3(0.0f,-0.25f,-0.2f); // ADS
	*e->matrix() = glm::gtx::transform2::lookAt(glm::vec3(0.0f), glm::vec3(0.0, 0.0f, -1000.0f) - gunpos, Axis::Y);
	//Matrix::translation(*e->matrix(), glm::vec3(0.2f,-0.25f,-0.4f));
	Matrix::translation(*e->matrix(), gunpos);
	//m_pPlayer->add(e);
	//m_pScene->add(e);
	//m_pScene->root()->add(e);
	
	// Particle System Demo (Fire)
	ParticleSystem::Particle particle;
	particle.size = 20.0f;
	particle.life = 1.0f;
	particle.color.set(1.0f, 0.6f, 0.3f); // fire
	particle.vel = glm::vec3(0.0f,1.0f,0.0f);
	ParticleSystem* ps = new ParticleSystem(new Texture("data/particles/particle.png", Texture::MIPMAPPED), particle, ParticleSystem::BLEND_ADDITIVE, 0.5f, 500.0f); //.25, 200
	ps->position(glm::vec3(0.0f, -0.5f, 0.0f));
	m_pScene->root()->add(ps);
	
	particle.size = 5.0f;
	particle.life = 10.0f;
	particle.vel = glm::vec3(0.0f,-1.0f,0.0f);
	particle.color.set(1.0f, 1.0f, 1.0f); // snow
	ps = new ParticleSystem(new Texture("data/particles/particle.png", Texture::MIPMAPPED), particle, ParticleSystem::BLEND_COLOR, 1.0f, 1000.0f);
	ps->position(glm::vec3(0.0f, 3.0f, -3.0f));
	m_pScene->root()->add(ps);
	
	//particle.size = 3000.0f;
	//particle.life = 2.0f;
	//particle.vel = glm::vec3(0.0f,-1.0f,0.0f);
	//particle.color.set(0.3f, 0.3f, 0.3f, 0.2f);
	//ps = new ParticleSystem(new Texture("data/particles/particle.png", Texture::MIPMAPPED), particle, ParticleSystem::BLEND_DARKEN, 1.0f, 5.0f);
	//Matrix::translate(*ps->matrix(), glm::vec3(5.0f, 1.0f, -3.0f));
	//m_pScene->root()->add(ps);

	// end particle system demo
	
	Light* light;
	// add three lights
	//m_pScene->add(light = new Light());
	//light->lightType(Light::DIRECTIONAL);
	//light->atten(glm::vec3(1.0f, 0.0f, 0.0f));
	//light->diffuse(Color(0.2f, 0.1f, 0.6f));
	
	// Colored Lights Demo
	m_pScene->add(light = new Light());
	light->move(glm::vec3(-5.0f, 0.0, 0.0f));
	light->atten(glm::vec3(0.0f, 0.0f, 0.01f));
	light->diffuse(Color(0.3f, 0.5f, 0.4f));
	
	m_pScene->add(light = new Light());
	light->move(glm::vec3(0.0f, 0.0, 0.0f));
	light->atten(glm::vec3(0.0f, 0.0f, 0.01f));
	light->diffuse(Color(0.3f, 0.4f, 0.5f));
	
	m_pScene->add(light = new Light());
	light->move(glm::vec3(5.0f, 0.0f, 0.0f));
	light->atten(glm::vec3(0.0f, 0.0f, 0.01f));
	light->diffuse(Color(0.5f, 0.4f, 0.3f));

	//m_pScene->add(light = new Light());
	//light->move(glm::vec3(-2.5f, 0.0, -5.0f));
	//light->atten(glm::vec3(0.0f, 0.0f, 0.5f));
	//light->diffuse(Color(0.8f, 0.4f, 0.6f));
	//m_pScene->add(light = new Light());
	//light->move(glm::vec3(-2.5f, 0.0, -5.0f));
	//light->diffuse(Color(0.5f, 0.5f, 0.0f));
	//light->atten(glm::vec3(0.0f, 0.0f, 0.5f));
	//m_pScene->add(light = new Light());
	//light->move(glm::vec3(2.5f, 0.0, -5.0f));
	//light->diffuse(Color(0.0f, 0.5f, 0.5f));
	//light->atten(glm::vec3(0.0f, 5.0f, 0.5f));
	//m_pScene->add(light = new Light());
	//light->move(glm::vec3(-5.0f, 0.0, -5.0f));
	//light->diffuse(Color(0.0f, 1.0f, 0.0f));
	//light->atten(glm::vec3(0.0f, 0.0f, 0.5f));
	//m_pScene->add(light = new Light());
	//light->move(glm::vec3(5.0f, 0.0, -5.0f));
	//light->diffuse(Color(0.0f, 0.0f, 0.5f));
	//light->atten(glm::vec3(0.0f, 0.0f, 0.5f));
	
	// Sound Demo
	//Sound* fire_sound = new Sound(
	//    new Audio::Buffer("data/sounds/fire.wav"),
	//    Audio::Source::F_LOOP | Audio::Source::F_AUTOPLAY);
	//fire_sound->source()->gain = 0.2f;
	//m_pScene->add(fire_sound);

	// Music Demo
	//m_spMusic.reset(new Audio::Stream("data/music/atmos1.ogg"));
	//m_spMusic->play();

	// "Compile" Physics
	m_pScene->compile();

	// Setup renderer states to be appropriate for mode
	m_pEngine->input()->hideMouse(true);
	Renderer::get().lighting(Renderer::ENABLE_LIGHTING);
	Renderer::get().shaders(Renderer::ENABLE_SHADERS);
}

GameState::~GameState()
{
	m_pEngine->input()->hideMouse(false);

	delete m_pScene;
}

void GameState::nullify()
{
	m_pEngine = NULL;
	m_pScene = NULL;

	m_bError = false;
	m_sError = "";
}

int GameState::logic(unsigned int advance)
{
	// perform logic for all objects in scene graph
	m_pScene->logic(advance);

	//m_spMusic->refresh();

	// set player as audio listener
	m_pPlayer->listen();
	
	return 0;
}

void GameState::render() const
{
	Renderer::get().viewport(Renderer::VIEW_PERSPECTIVE, 0.0f, 0.0f, 1.0f, 1.0f);
	m_pScene->render(m_pPlayer);

	//m_pEngine->renderer()->viewport(Renderer::VIEW_PERSPECTIVE, 0.0f, 0.5f, 1.0f, 0.5f);
	//m_pScene->render();
}

