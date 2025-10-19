#pragma once

int GetAppIdByTitleId(const char* TitleId);
int StartApp(const char* titleId, char** args, int userId);
int StartRestartApp(const char* titleId, char** args, int userId);
int StopApp(const char* titleId);