#include "plugin.hpp"


Plugin* pluginInstance;


void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	p->addModel(modelFslop);
	p->addModel(modelCpole_tanh);
	p->addModel(modelCjpoles_tanh);
    p->addModel(modelRclop);
	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
