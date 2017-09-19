#pragma once

namespace Render
{
	class Device;
	class RenderTarget;
}

class PostProcess
{
public:
	PostProcess(Render::Device* device);
	~PostProcess();

	void begin(Render::Device* device, const Render::RenderTarget* renderTarget);
	void end(Render::Device* device, const Render::RenderTarget* renderTarget);

private:
	struct PrivateData;
	PrivateData* m;
};
