#include "plugin.hpp"


struct Fslop : Module {
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

	Fslop() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(POT0_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN0_INPUT, "");
		configOutput(OUT0_OUTPUT, "");
	}
    
    float o0,o1;
    
	void process(const ProcessArgs& args) override
    {
        float c=params[POT0_PARAM].getValue();
        float i0=inputs[IN0_INPUT].getVoltage();
        
        //static float o1=0;  //no static variables !!!!!!!
        //float o0;
        
        // https://yehar.com/blog/?p=121
        //A collection of IIR filters
        //Fastest and simplest "lowpass" ever!
        //  c = 0..1  (1 = passes all, 0 = passes nothing)
        //  output(t) = output(t-1) + c*(input(t)-output(t-1))
        
        o0=o1+c*(i0-o1);
        o1=o0;
        
        outputs[OUT0_OUTPUT].setVoltage(o0);
	}
};


struct FslopWidget : ModuleWidget {
	FslopWidget(Fslop* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/fslop.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 21.253)), module, Fslop::POT0_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 64.25)), module, Fslop::IN0_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 91.231)), module, Fslop::OUT0_OUTPUT));
	}
};


Model* modelFslop = createModel<Fslop, FslopWidget>("fslop");
