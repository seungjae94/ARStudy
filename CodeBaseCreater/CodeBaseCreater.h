#pragma once
#include <string>
#include <list>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "tinyxml2.h"

namespace fs = std::filesystem;
namespace tx = tinyxml2;

enum class EAnswerType
{
	Int = 0,
	String = 1,
	Float = 2,
};

class CodeBaseCreater
{
public:
	static void RegisterUsers(const std::list<std::string>& _Users);
	static void Create(std::string_view _WorkbookName, std::string_view _ProblemName, EAnswerType _AnswerType);
protected:

private:
	// constructor destructor
	CodeBaseCreater();
	~CodeBaseCreater();

	// delete Function
	CodeBaseCreater(const CodeBaseCreater& _Other) = delete;
	CodeBaseCreater(CodeBaseCreater&& _Other) noexcept = delete;
	CodeBaseCreater& operator=(const CodeBaseCreater& _Other) = delete;
	CodeBaseCreater& operator=(CodeBaseCreater&& _Other) noexcept = delete;

	static std::list<std::string> Users;
	static std::string ProjectName;

	// 매번 초기화
	static std::string WorkbookName;
	static std::string ProblemName;
	static EAnswerType AnswerType;
	static std::list<std::string> FileNames;

	static void CreateCodes();
	static void AddCodesToProject();
	static void ModifyProjectFile();
	static void ModifyFilterFile();

	static fs::path GetProjectPath();
	static std::string GetTypeName(EAnswerType _Type);
	static std::string GenerateNewGuid();

	static tx::XMLElement* GetFilterItemGroup(tx::XMLDocument& _Dom);
	static tx::XMLElement* GetClIncludeItemGroup(tx::XMLDocument& _Dom);
	static bool IsFilterDefined(tx::XMLDocument& _Dom, std::string_view _FilterName);
	static void DefineFilter(tx::XMLDocument& _Dom, std::string_view _FilterName);
};

