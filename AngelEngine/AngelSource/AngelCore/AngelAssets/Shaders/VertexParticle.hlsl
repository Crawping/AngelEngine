#include"ParamParticle.hlsli"

GSParticleIn main(VSParticleIn input)
{
	GSParticleIn output;

	output.Pos = input.Pos;
	output.Life = input.Life;
	output.Size = input.Size;
	output.Vel = input.Vel;
	return output;
}
