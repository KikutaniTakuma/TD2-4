#pragma once

class ImGuiManager {
private:
	ImGuiManager();
	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager(ImGuiManager&&) = delete;

	ImGuiManager& operator=(const ImGuiManager&) = delete;
	ImGuiManager& operator=(ImGuiManager&&) = delete;
public:
	~ImGuiManager();

public:
	static ImGuiManager* const GetInstance();
	static void Initialize();
	static void Finalize();

private:
	static ImGuiManager* instance_;

public:
	void Start();

	void End();
};