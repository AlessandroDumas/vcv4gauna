#include "plugin.hpp"


struct CbiteB : Module {
	enum ParamId {
		POT0_PARAM,
		POT1_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		INPUTS_LEN
	};
	enum OutputId {
		OUT0_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	CbiteB() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(POT0_PARAM, 0.f, 1.f, 0.f, "");
		configParam(POT1_PARAM, 0.f, 1.f, 0.f, "");
		configOutput(OUT0_OUTPUT, "");
	}
    
    unsigned int tk;

	void process(const ProcessArgs& args) override
    {
        int p=127*params[POT0_PARAM].getValue();
        int k=1023*params[POT1_PARAM].getValue();
        
        unsigned int c;
        uint16_t t;
        
        t=tk>>8;
        
        c=(t>>4)*((t<<2)%0xff)*(t|123)*p;
        c=c&0xFF;
        
        tk=(tk+k-512);
        tk=tk&0xFFFFFF;
        
        outputs[OUT0_OUTPUT].setVoltage((float)((int)c-0x1F)/(float)0x1FF*5.0);

	}
};


struct CbiteBWidget : ModuleWidget {
	CbiteBWidget(CbiteB* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/cbiteB.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 21.253)), module, CbiteB::POT0_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 42.42)), module, CbiteB::POT1_PARAM));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 91.231)), module, CbiteB::OUT0_OUTPUT));
	}
};


Model* modelCbiteB = createModel<CbiteB, CbiteBWidget>("cbiteB");
