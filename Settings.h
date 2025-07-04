#ifndef SETTINGS_H
#define SETTINGS_H

class Settings
{
private:
	static Settings* instance;

	Settings() {};
	Settings(const Settings&) = delete;
	Settings& operator=(const Settings&) = delete;

public:

	int SCREEN_WIDTH = 1920;
	int SCREEN_HEIGHT = 1080;
	int FRAME_RATE = 60;

	static Settings* GetInstance()
	{
		if (!instance)
			instance = new Settings();
		return instance;
	}

	void SetScreenSize(int width, int height)
	{
		SCREEN_WIDTH = width;
		SCREEN_HEIGHT = height;
	}
};

#endif
