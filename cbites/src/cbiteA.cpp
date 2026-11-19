#include "plugin.hpp"


struct CbiteA : Module {
	enum ParamId {
		POT0_PARAM,
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

	CbiteA() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(POT0_PARAM, 0.f, 1.f, 0.f, "");
		configOutput(OUT0_OUTPUT, "");
	}
    
    int t;

	void process(const ProcessArgs& args) override
    {
        int p=127*params[POT0_PARAM].getValue();
        
        unsigned int c;
        
        c=(t>>4)*((t<<2)%0xff)*(t|123)*p;
        c=c&0xFF;
        
        t++;
        t=t&0xFFFF;
        
        outputs[OUT0_OUTPUT].setVoltage((float)((int)c-0x1F)/(float)0x1FF*5.0);
	}
};


struct CbiteAWidget : ModuleWidget {
	CbiteAWidget(CbiteA* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/cbiteA.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 21.253)), module, CbiteA::POT0_PARAM));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 91.231)), module, CbiteA::OUT0_OUTPUT));
	}
};


Model* modelCbiteA = createModel<CbiteA, CbiteAWidget>("cbiteA");
