#include "plugin.hpp"


struct Rclop : Module {
	enum ParamId {
		POT0_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN0_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT0_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Rclop() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(POT0_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN0_INPUT, "");
		configOutput(OUT0_OUTPUT, "");
	}

    float o0,o1;
    
	void process(const ProcessArgs& args) override
    {
        float p=params[POT0_PARAM].getValue();
        float i0=inputs[IN0_INPUT].getVoltage();
        
        //https://community.vcvrack.com/t/could-someone-give-me-advice-on-developing-modules-for-vcv-rack/22000/7
        //no static variables ! (shared )
        //static float o1=0;
        //float o0;
        
        p=p*p*p;  //potenziometro meno sensibile a inizio corsa
        //o0=i0*p+o1*(1-p);
        o0=o1+p*(i0-o1);
        o1=o0;
        
        outputs[OUT0_OUTPUT].setVoltage(o0);
    }
};


struct RclopWidget : ModuleWidget {
	RclopWidget(Rclop* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/rclop.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 21.253)), module, Rclop::POT0_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 64.25)), module, Rclop::IN0_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 91.231)), module, Rclop::OUT0_OUTPUT));
	}
};


Model* modelRclop = createModel<Rclop, RclopWidget>("rclop");
