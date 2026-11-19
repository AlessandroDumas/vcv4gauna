#include "plugin.hpp"


struct Cjpoles_tanh : Module {
	enum ParamId {
		POT0_PARAM,
		POT1_PARAM,
		POT2_PARAM,
		POT3_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CV_IN0_INPUT,
		CV_IN1_INPUT,
		CV_IN2_INPUT,
		CV_IN3_INPUT,
		IN0_RE_INPUT,
		IN0_IM_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT0_RE_OUTPUT,
		OUT0_IM_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Cjpoles_tanh() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(POT0_PARAM, 0.f, 1.f, 0.5, "");
		configParam(POT1_PARAM, -1.f, 1.f, 0.f, "");
		configParam(POT2_PARAM, -1.f, 1.f, 0.f, "");
		configParam(POT3_PARAM, -1.f, 1.f, 0.f, "");
		configInput(CV_IN0_INPUT, "");
		configInput(CV_IN1_INPUT, "");
		configInput(CV_IN2_INPUT, "");
		configInput(CV_IN3_INPUT, "");
		configInput(IN0_RE_INPUT, "");
		configInput(IN0_IM_INPUT, "");
		configOutput(OUT0_RE_OUTPUT, "");
		configOutput(OUT0_IM_OUTPUT, "");
	}

    float out[2][2]; //out[RE/IM][z^-t]
    float outj[2][2];

    void process(const ProcessArgs& args) override
    {
        float in[2]; //in[RE/IM]
        float coeff[2]; // [RE/IM]
        
        // POTs & CVs
        float gain=0.2*params[POT0_PARAM].getValue()+inputs[CV_IN0_INPUT].getVoltage()/5.0;
        //gain=max(0,min(10,gain));
        gain=clamp(gain,0.0,10.0);
        float offset=0.2*(params[POT1_PARAM].getValue()+inputs[CV_IN1_INPUT].getVoltage()/5.0);
        float freq_V=5.0*params[POT2_PARAM].getValue()+inputs[CV_IN2_INPUT].getVoltage();
        float freq=441.0*std::pow(2.0,freq_V);
        freq=clamp(freq,0.0,args.sampleRate/2.0);
        float damp=8.0*params[POT3_PARAM].getValue()+inputs[CV_IN3_INPUT].getVoltage()/5.0;
        damp=std::pow(damp,3);
        
        // INs
        in[0]=inputs[IN0_RE_INPUT].getVoltage();
        in[1]=inputs[IN0_IM_INPUT].getVoltage();
        
        //polo complesso
        // p=a+ib
        coeff[0]=cos(2.0*M_PI*freq/args.sampleRate)*(1-damp/1000);
        coeff[1]=sin(2.0*M_PI*freq/args.sampleRate)*(1-damp/1000);
        
        in[0]*=gain;
        in[1]*=gain;
        
        in[0]+=random::normal()*1e-6;
        in[1]+=random::normal()*1e-6;
        
        // funzione di trasferimento
        // H(z)=1/(z-p)
        // 1=|H(0)|=|a0/(1-p)|
        // coefficiente di normalizzazione
        // a0=|1-p|
        float a0=sqrt((1-coeff[0])*(1-coeff[0])+coeff[1]*coeff[1]);
        
        // b1=p
        // output(t+0) = a0*input(t) +b1*output(t-1)
        
        //moltiplicazione complessa e nonlinearità
        //RE
        out[0][0]=a0*in[0]+coeff[0]*out[0][1]-out[1][1]*coeff[1];
        out[0][0]=tanh(out[0][0]+offset)-tanh(offset);
//        out[0][0]=clamp(out[0][0],-1.0,1.0);
        //IM
        out[1][0]=a0*in[1]+coeff[1]*out[0][1]+out[1][1]*coeff[0];
        out[1][0]=tanh(out[1][0]+offset)-tanh(offset);
//        out[1][0]=clamp(out[1][0],-1.0,1.0);
        //mem
        out[0][1]=out[0][0];
        out[1][1]=out[1][0];
        
        //polo complesso coniugato p*=a-ib
        //RE
        coeff[1]=-coeff[1];
        in[0]=out[0][0];
        in[1]=out[1][0];
        outj[0][0]=a0*in[0]+coeff[0]*outj[0][1]-outj[1][1]*coeff[1];
        outj[0][0]=tanh(outj[0][0]+offset)-tanh(offset);
//        outj[0][0]=clamp(outj[0][0],-1.0,1.0);
        //IM
        outj[1][0]=a0*in[1]+coeff[1]*outj[0][1]+outj[1][1]*coeff[0];
        outj[1][0]=tanh(outj[1][0]+offset)-tanh(offset);
//        outj[1][0]=clamp(outj[1][0],-1.0,1.0);
        //mem
        outj[0][1]=outj[0][0];
        outj[1][1]=outj[1][0];
        
        // OUTs
        outputs[OUT0_RE_OUTPUT].setVoltage(10.0*outj[0][0]);
        outputs[OUT0_IM_OUTPUT].setVoltage(10.0*outj[1][0]);
	}
};


struct Cjpoles_tanhWidget : ModuleWidget {
	Cjpoles_tanhWidget(Cjpoles_tanh* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/cjpoles_tanh.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 19.8)), module, Cjpoles_tanh::POT0_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 37.58)), module, Cjpoles_tanh::POT1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 55.36)), module, Cjpoles_tanh::POT2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 73.14)), module, Cjpoles_tanh::POT3_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.48, 19.8)), module, Cjpoles_tanh::CV_IN0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.48, 37.58)), module, Cjpoles_tanh::CV_IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.48, 55.36)), module, Cjpoles_tanh::CV_IN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.48, 73.14)), module, Cjpoles_tanh::CV_IN3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 90.92)), module, Cjpoles_tanh::IN0_RE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.48, 90.92)), module, Cjpoles_tanh::IN0_IM_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 108.7)), module, Cjpoles_tanh::OUT0_RE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30.48, 108.7)), module, Cjpoles_tanh::OUT0_IM_OUTPUT));
	}
};


Model* modelCjpoles_tanh = createModel<Cjpoles_tanh, Cjpoles_tanhWidget>("cjpoles_tanh");
