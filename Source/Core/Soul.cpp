/////////////////////////Includes/////////////////////////////////

#include "Rasterer/RasterManager.h"
#include "Transput/Configuration/Settings.h"
#include "Core/Utility/Logger.h"
#include "Physics/PhysicsEngine.h"
#include "Parallelism/Compute/ComputeManager.h"
#include "Display/Window/ManagerInterface.h"
#include "Composition/Event/EventManager.h"
#include "Transput/Input/InputManager.h"
#include "Tracer/RayEngine.h"
#include "SoulApplication.h"

namespace Soul {

	/* //////////////////////Variables and Declarations//////////////////. */

	//Application app;

	double engineRefreshRate;
	double allotedRenderTime;
	bool running = true;


	/* //////////////////////Synchronization///////////////////////////. */

	void SynchCPU() {
		//Scheduler::Block(); //TODO Implement MT calls
	}

	void SynchGPU() {
		//CudaCheck(cudaDeviceSynchronize());
	}

	void SynchSystem() {
		SynchCPU();
		SynchGPU();
	}


	/////////////////////////Hints and Toggles///////////////////////////



	/////////////////////////Core/////////////////////////////////
	// 

	/* Initializes the engine. */
	void Initialize() {

		//create the listener for threads initializeing
		EventManager::Listen("Thread", "Initialize", []()
		{
			ComputeManager::Instance().InitThread();
		});

		//open the config file for the duration of the runtime
		Settings::Read("config.ini", TEXT);

		//extract all available GPU devices
		ComputeManager::Instance().ExtractDevices();

		//set the error callback
		glfwSetErrorCallback([](int error, const char* description) {
			S_LOG_FATAL("GLFW Error occured, Error ID:", error, " Description:", description);
		});

		//Initialize glfw context for Window handling
		const int	didInit = glfwInit();

		if (!didInit) {
			S_LOG_FATAL("GLFW did not initialize");
		}

		RasterManager::Instance();

		Settings::Get("Engine.Delta_Time", 1 / 60.0, engineRefreshRate);
		Settings::Get("Engine.Alloted_Render_Time", 0.01, allotedRenderTime);

	}

	/* Call to deconstuct both the engine and its dependencies. */
	void Terminate() {
		Soul::SynchSystem();

		//Write the settings into a file
		Settings::Write("config.ini", TEXT);

		//destroy glfw, needs to wait on the window manager
		glfwTerminate();

		//extract all available GPU devices
		ComputeManager::Instance().DestroyDevices();

	}


	/* Ray pre process */
	void RayPreProcess() {

		//RayEngine::Instance().PreProcess();

	}

	/* Ray post process */
	void RayPostProcess() {

		//RayEngine::Instance().PostProcess();

	}

	/* Rasters this object. */
	void Raster() {

		//Backends should handle multithreading
		ManagerInterface::Instance().Draw();

	}

	/* Warmups this object. */
	void Warmup() {

		glfwPollEvents();

		//for (auto& scene : scenes) {
		//	scene->Build(engineRefreshRate);
		//}

	}

	/* Early frame update. */
	void EarlyFrameUpdate() {

		EventManager::Emit("Update", "EarlyFrame");

	}
	/* Late frame update. */
	void LateFrameUpdate() {

		EventManager::Emit("Update", "LateFrame");

	}

	/* Early update. */
	void EarlyUpdate() {

		//poll events before this update, making the state as close as possible to real-time input
		glfwPollEvents();

		//poll input after glfw processes all its callbacks (updating some input states)
		InputManager::Poll();

		EventManager::Emit("Update", "Early");

		//Update the engine cameras
		//RayEngine::Instance().Update();

		//pull cameras into jobs
		EventManager::Emit("Update", "Job Cameras");

	}

	/* Late update. */
	void LateUpdate() {

		EventManager::Emit("Update", "Late");

	}

	/* Runs this object. */
	void Run()
	{

		Warmup();

		//setup timer info
		double t = 0.0f;
		double currentTime = glfwGetTime();
		double accumulator = 0.0f;

		while (running && !ManagerInterface::Instance().ShouldClose()) {

			//start frame timers
			double newTime = glfwGetTime();
			double frameTime = newTime - currentTime;

			if (frameTime > 0.25) {
				frameTime = 0.25;
			}

			currentTime = newTime;
			accumulator += frameTime;

			EarlyFrameUpdate();

			//consumes time created by the renderer
			while (accumulator >= engineRefreshRate) {

				EarlyUpdate();

				/*for (auto& scene : scenes) {
					scene->Build(engineRefreshRate);
				}*/
				/*
				for (auto const& scene : scenes){
					PhysicsEngine::Process(scene);
				}*/

				LateUpdate();

				t += engineRefreshRate;
				accumulator -= engineRefreshRate;
			}


			LateFrameUpdate();

			RayPreProcess();

			//RayEngine::Instance().Process(*scenes[0], engineRefreshRate);

			RayPostProcess();

			Raster();

		}
	}
}

/*
 *    //////////////////////User Interface///////////////////////////.
 *    @param	pressType	Type of the press.
 */

void SoulSignalClose() {
	Soul::running = false;
	ManagerInterface::Instance().SignalClose();
}

/* Soul run. */
void SoulRun() {
	Soul::Run();
}

/*
 *    Gets delta time.
 *    @return	The delta time.
 */

double GetDeltaTime() {
	return Soul::engineRefreshRate;
}

/* Initializes Soul. This should be the first command in a program. */
void SoulInit() {
	Soul::Initialize();
}

/* Soul terminate. */
void SoulTerminate() {
	Soul::Terminate();
}

/*
 *    Submit scene.
 *    @param [in,out]	scene	If non-null, the scene.
 */

void SubmitScene(Scene* scene) {
	//Soul::scenes.push_back(std::unique_ptr<Scene>(scene));
}


int main()
{

	
	//SoulInit();

	//EventManager::Listen("Input", "ESCAPE", [](keyState state) {
	//	if (state == RELEASE) {
	//		SoulSignalClose();
	//	}
	//});

	//uint xSize;
	//Settings::Get("MainWindow.Width", uint(800), xSize);
	//uint ySize;
	//Settings::Get("MainWindow.Height", uint(450), ySize);
	//uint xPos;
	//Settings::Get("MainWindow.X_Position", uint(0), xPos);
	//uint yPos;
	//Settings::Get("MainWindow.Y_Position", uint(0), yPos);
	//int monitor;
	//Settings::Get("MainWindow.Monitor", 0, monitor);

	//WindowType type;
	//int typeCast;
	//Settings::Get("MainWindow.Type", static_cast<int>(WINDOWED), typeCast);
	//type = static_cast<WindowType>(typeCast);

	//AbstractWindow* mainWindow = ManagerInterface::Instance().CreateWindow(type, "main", monitor, xPos, yPos, xSize, ySize);

	//ManagerInterface::Instance().SetWindowLayout(mainWindow, new SingleLayout(new RenderWidget()));

	//SoulRun();

	//SoulTerminate();

	//return EXIT_SUCCESS;

}