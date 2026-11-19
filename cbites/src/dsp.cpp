#include "plugin.hpp"


struct Dsp : Module {
	enum ParamId {
		POT0_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN0_INPUT,
		CV_IN0_INPUT,
		IN1_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT0_OUTPUT,
		OUT1_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Dsp() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(POT0_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN0_INPUT, "");
		configInput(CV_IN0_INPUT, "");
		configInput(IN1_INPUT, "");
		configOutput(OUT0_OUTPUT, "");
		configOutput(OUT1_OUTPUT, "");
	}

    //memoria per delay / riverbero
    #define mem_len 16384
    int16_t mem[2][mem_len];
    uint16_t t;
    
	void process(const ProcessArgs& args) override
    {
        int16_t vin[2],vout[2];  //Audio in/out
        int16_t cvin[1];         //CV e potenziometri

        int32_t mac;
/*
        vin[0]=(ADC_RDATA(ADC0)<<2)-8192;  //u12->s14
        vin[1]=(ADC_RDATA(ADC1)<<2)-8192;    
        cvin[0]=ADC_RDATA(ADC2)-2048;  //u12->s12
*/
        
        float ain[2], aout[2];
        
        ain[0]=inputs[IN0_INPUT].getVoltage();
        ain[1]=inputs[IN1_INPUT].getVoltage();
        cvin[0]=4095*params[POT0_PARAM].getValue()-2048;

        //HIPASS input
        //vin0=...
        
        //Sampler
        //float ph;
        //ph=cvin[0]

        //offset & quantizzazione ADC 12bit
        vin[0]=(ain[0]+1.65)/3.3*4096; // u12b
        vin[0]=clamp(vin[0],0,4095);  // 1.0=4096 !
        vin[0]=(vin[0]<<2)-8192;    // u12->s14
        vin[1]=(ain[1]+1.65)/3.3*4096;
        vin[1]=clamp(vin[1],0,4095);
        vin[1]=(vin[1]<<2)-8192;
        
        
        //CH0
        // delay taps (=>~reverb)
        mac=0;
        mac+=vin[0]*4;
        mac+=+(mem[0][(mem_len-344+t)%mem_len]);
        mac+=-(mem[0][(mem_len-384+t)%mem_len]);
        mac+=+(mem[0][(mem_len-1656+t)%mem_len]);
        mac+=+(mem[0][(mem_len-3864+t)%mem_len]);
        mac+=-(mem[0][(mem_len-5492+t)%mem_len]);
        mac+=+(mem[0][(mem_len-6256+t)%mem_len]);
        mac+=-(mem[0][(mem_len-10120+t)%mem_len]);
        mac+=+(mem[0][(mem_len-12512+t)%mem_len]);
        mac+=+(mem[0][(mem_len-15360+t)%mem_len]);
        mac+=+(mem[1][(mem_len-1228+t)%mem_len]);
        mac+=+(mem[1][(mem_len-1796+t)%mem_len]);
        mac+=-(mem[1][(mem_len-12544+t)%mem_len]);
        mac+=+(mem[1][(mem_len-15704+t)%mem_len]);

        mac=mac/8;
        
        //clip s14
        if(mac<-8192)
            mac=-8192;
        if(mac>8191)
            mac=8191;

        //IIR (feedback)
        mem[0][t]=-mac;
        vout[0]=mac;

        //CH1
        // delay taps (=>~reverb)
        mac=0;
        mac+=vin[1]*4;
        mac+=-(mem[1][(mem_len-464+t)%mem_len]);
        mac+=+(mem[1][(mem_len-784+t)%mem_len]);
        mac+=+(mem[1][(mem_len-876+t)%mem_len]);
        mac+=-(mem[1][(mem_len-3984+t)%mem_len]);
        mac+=+(mem[1][(mem_len-5476+t)%mem_len]);
        mac+=-(mem[1][(mem_len-6296+t)%mem_len]);
        mac+=+(mem[1][(mem_len-10080+t)%mem_len]);
        mac+=+(mem[1][(mem_len-12524+t)%mem_len]);
        mac+=+(mem[1][(mem_len-15680+t)%mem_len]);
        mac+=-(mem[0][(mem_len-1372+t)%mem_len]);
        mac+=-(mem[0][(mem_len-3656+t)%mem_len]);
        mac+=-(mem[0][(mem_len-12552+t)%mem_len]);
        mac+=+(mem[0][(mem_len-15360+t)%mem_len]);

        mac=mac/8;
        
        //clip s14
        if(mac<-8192)
            mac=-8192;
        if(mac>8191)
            mac=8191;

        //IIR (feedback)
        mem[1][t]=-mac;
        vout[1]=mac;
        
        t=(t+1)%mem_len;
        
/*
        //uscite PWM
        TIMER_CH0CV(TIMER2)=(uint16_t)(vout[0]+8192)>>7; // 14bit -> 7bit MSB
        TIMER_CH1CV(TIMER2)=(uint16_t)(vout[1]+8192)>>7; // 14bit -> 7bit MSB
        //uscite PWM lsb
        TIMER_CH2CV(TIMER2)=(uint16_t)(vout[0]+8192)&0x007f; // 14bit -> 7bit LSB
        TIMER_CH3CV(TIMER2)=(uint16_t)(vout[1]+8192)&0x007f; // 14bit -> 7bit LSB
        
        //super jitter/mod/spread spectrum/quantum dithering.
        //TIMER_CAR(TIMER1)=TIMER1_PERIOD+(vout[0]+vout[1])/8-cvin[0]*8;  //change period...
        TIMER_CAR(TIMER1)=(TIMER1_PERIOD+(vout[0]+vout[1])/8)*512/(cvin[0]+2048+256);  //change period...
        TIMER_CH1CV(TIMER1)=(TIMER_CAR(TIMER1)+1)/2-1;  //... and adjust pulse width
*/      
        
        //quantizzazzione DAC (PWM 14bit)
        //14->14
        aout[0]=(vout[0]+8192)/16384.0*3.3;
        aout[1]=(vout[0]+8192)/16384.0*3.3;
        //hipass (& lowpass)
        //....
        outputs[OUT0_OUTPUT].setVoltage(aout[0]-1.65);
        outputs[OUT1_OUTPUT].setVoltage(aout[1]-1.65);
        
	}
};


struct DspWidget : ModuleWidget {
	DspWidget(Dsp* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/dsp.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(29.868, 79.439)), module, Dsp::POT0_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.0, 21.068)), module, Dsp::IN0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(28.24, 21.068)), module, Dsp::CV_IN0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.0, 36.308)), module, Dsp::IN1_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(13.0, 92.188)), module, Dsp::OUT0_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.999, 107.428)), module, Dsp::OUT1_OUTPUT));
	}
};


Model* modelDsp = createModel<Dsp, DspWidget>("dsp");
