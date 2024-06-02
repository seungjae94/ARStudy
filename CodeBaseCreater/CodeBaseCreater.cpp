#include "CodeBaseCreater.h"

std::list<std::string> CodeBaseCreater::Users;
std::string CodeBaseCreater::ProjectName = "assignment";

std::string CodeBaseCreater::WorkbookName;
std::string CodeBaseCreater::ProblemName;
EAnswerType CodeBaseCreater::AnswerType;
std::list<std::string> CodeBaseCreater::FileNames;

void CodeBaseCreater::RegisterUsers(const std::list<std::string>& _Users)
{
    Users = _Users;
}

void CodeBaseCreater::Create(std::string_view _WorkbookName, std::string_view _ProblemName, EAnswerType _AnswerType)
{
    WorkbookName = _WorkbookName;
    ProblemName = _ProblemName;
    AnswerType = _AnswerType;
    FileNames.clear();

    CreateCodes();
    AddCodesToProject();
}

CodeBaseCreater::CodeBaseCreater()
{
}

CodeBaseCreater::~CodeBaseCreater()
{
}

void CodeBaseCreater::CreateCodes()
{
    std::string TypeName = GetTypeName(AnswerType);

    for (std::string_view User : Users)
    {
        std::string FileName = std::string(User) + "_" + ProblemName.data() + ".cpp";
        FileNames.push_back(FileName);

        std::ofstream File{ (GetProjectPath() / FileName).string() };
        File << "#include \"PreCompile.h\"" << "\n" << "\n";
        File << TypeName << " " << "solution() {" << "\n" << "\n";
        File << "\treturn " << TypeName << "();" << "\n";
        File << "}" << "\n" << "\n";
        File << "SUBMIT(" << TypeName << ", solution)" << std::endl;
        File.close();
    }
}

void CodeBaseCreater::AddCodesToProject()
{
    ModifyProjectFile();
    ModifyFilterFile();
}

void CodeBaseCreater::ModifyProjectFile()
{
    fs::path VcxprojPath = GetProjectPath() / (ProjectName + ".vcxproj");

    // XML 파일을 읽어서 XMLDocument 객체를 생성한다.
    tx::XMLDocument Doc;
    Doc.LoadFile(VcxprojPath.string().c_str());

    // <Project><ItemGroup> 요소 중 <ClInclude> 요소를 자식으로 갖는 요소를 찾는다.
    tx::XMLElement* Elem = Doc.FirstChildElement("Project")->FirstChildElement("ItemGroup");

    while (true)
    {
        if (nullptr != Elem->FirstChildElement("ClInclude"))
        {
            break;
        }

        Elem = Elem->NextSiblingElement("ItemGroup");
    }

    // 찾은 <Project><ItemGroup> 요소에 새로운 <ClInclude> 요소를 삽입한다.
    for (const std::string& FileName : FileNames)
    {
        tx::XMLElement* NewElem = Elem->InsertNewChildElement("ClInclude");
        NewElem->SetAttribute("Include", FileName.c_str());
        NewElem->InsertNewChildElement("SubType");
    }

    // XMLDocument 객체를 파일로 저장한다.
    Doc.SaveFile(VcxprojPath.string().c_str());
}

void CodeBaseCreater::ModifyFilterFile()
{
    fs::path VcxprojFiltersPath = GetProjectPath() / (ProjectName + ".vcxproj.filters");

    // XML 파일을 읽어서 XMLDocument 객체를 생성한다.
    tx::XMLDocument Dom;
    Dom.LoadFile(VcxprojFiltersPath.string().c_str());

    // <Project><ItemGroup> 요소 중 <ClInclude> 요소를 자식으로 갖는 요소
    tx::XMLElement* ClIncludeItemGroup = GetClIncludeItemGroup(Dom);

    // <문제집> 필터 생성
    // <Filter Include="01_Queue"><UniqueIdentifier>{GUID}</></>
    if (false == IsFilterDefined(Dom, WorkbookName))
    {
        DefineFilter(Dom, WorkbookName);
    }

    for (std::string_view User : Users)
    {
        // <문제집/이름> 필터 생성
        std::string Name = std::string(WorkbookName) + "\\" + User.data();
        
        if (false == IsFilterDefined(Dom, Name))
        {
            DefineFilter(Dom, Name);
        }

        Name = std::string(WorkbookName) + "\\" + User.data() + "\\" + ProblemName.data();

        if (false == IsFilterDefined(Dom, Name))
        {
            DefineFilter(Dom, Name);
        }

        std::string FileName = std::string(User) + "_" + ProblemName.data() + ".cpp";

        tx::XMLElement* ClIncludeElem = ClIncludeItemGroup->InsertNewChildElement("ClInclude");
        ClIncludeElem->SetAttribute("Include", FileName.c_str());
        tx::XMLElement* ClIncludeFilterElem = ClIncludeElem->InsertNewChildElement("Filter");
        ClIncludeFilterElem->SetText(Name.c_str());
    }

    // XMLDocument 객체를 파일로 저장한다.
    Dom.SaveFile(VcxprojFiltersPath.string().c_str());
}

fs::path CodeBaseCreater::GetProjectPath()
{
    fs::path ProjectPath = fs::current_path();
    ProjectPath = ProjectPath.parent_path();
    ProjectPath /= ProjectName;
    return ProjectPath;
}

std::string CodeBaseCreater::GetTypeName(EAnswerType _Type)
{
    switch (_Type)
    {
    case EAnswerType::Int:
        return "int";
    case EAnswerType::String:
        return "std::string";
    case EAnswerType::Float:
        return "float";
    default:
        return "std::string";
    }
}

std::string CodeBaseCreater::GenerateNewGuid()
{
    GUID NewGuid;
    HRESULT Result = CoCreateGuid(&NewGuid);

    wchar_t* GuidBuffer = nullptr;
    Result = StringFromCLSID(NewGuid, &GuidBuffer);

    std::string GuidString;
    std::wstring GuidWString = GuidBuffer;
    for (wchar_t WCh : GuidWString)
    {
        GuidString += static_cast<char>(WCh);
    }

    ::CoTaskMemFree(GuidBuffer);

    return GuidString;
}

tx::XMLElement* CodeBaseCreater::GetFilterItemGroup(tx::XMLDocument& _Dom)
{
    tx::XMLElement* ItemGroup = _Dom.FirstChildElement("Project")->FirstChildElement("ItemGroup");

    while (true)
    {
        if (nullptr != ItemGroup->FirstChildElement("Filter"))
        {
            break;
        }

        ItemGroup = ItemGroup->NextSiblingElement("ItemGroup");
    }

    return ItemGroup;
}

tx::XMLElement* CodeBaseCreater::GetClIncludeItemGroup(tx::XMLDocument& _Dom)
{
    tx::XMLElement* ItemGroup = _Dom.FirstChildElement("Project")->FirstChildElement("ItemGroup");

    while (true)
    {
        if (nullptr != ItemGroup->FirstChildElement("ClInclude"))
        {
            break;
        }

        ItemGroup = ItemGroup->NextSiblingElement("ItemGroup");
    }

    return ItemGroup;
}

bool CodeBaseCreater::IsFilterDefined(tx::XMLDocument& _Dom, std::string_view _FilterName)
{
    tx::XMLElement* FilterItemGroup = _Dom.FirstChildElement("Project")->FirstChildElement("ItemGroup");

    tx::XMLElement* Filter = FilterItemGroup->FirstChildElement("Filter");
    if (nullptr == Filter)
    {
        return false;
    }

    while (true)
    {
        std::string FilterName = Filter->Attribute("Include");
        if (FilterName == _FilterName)
        {
            return true;
        }

        Filter = Filter->NextSiblingElement("Filter");

        if (nullptr == Filter)
        {
            break;
        }
    }

    return false;
}

void CodeBaseCreater::DefineFilter(tx::XMLDocument& _Dom, std::string_view _FilterName)
{
    tx::XMLElement* FilterDefItemGroup = _Dom.FirstChildElement("Project")->FirstChildElement("ItemGroup");

    tx::XMLElement* Filter = FilterDefItemGroup->InsertNewChildElement("Filter");
    Filter->SetAttribute("Include", _FilterName.data());
    tx::XMLElement* UID = Filter->InsertNewChildElement("UniqueIdentifier");
    UID->SetText(GenerateNewGuid().c_str());
}
