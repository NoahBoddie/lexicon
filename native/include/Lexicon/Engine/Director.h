#pragma once 
#include "Lexicon/Engine/Project.h"
#include "Lexicon/Engine/ParserTest.h"
#include "Lexicon/Engine/DirectiveRecord.h"
#include "Lexicon/Interfaces/ProjectClient.h"
namespace LEX
{
    class Script;
    class Project;

    struct Director;

    struct PostProcessData
    {
        std::unordered_map<std::string, std::string> values;
    };

    using PostProcessFn = void(*)(Script*, Director*, PostProcessData*);

    struct PostProcess
    {
        PostProcessFn callback;
        PostProcessData data;
    };





    struct Director
    {
    public:
        using iterator = std::string::iterator;

        static constexpr auto _max = max_value<Line>;

    private:

        struct ScriptLine
        {
            ScriptLine(iterator b, iterator e) : begin{ b }, end{ e } {}

            iterator begin;
            iterator end;


            bool enabled = true;	//If it's not enabled it will remove everything within the line but keep the line. Will be disabled in reverse
            //
        };

        void Init()
        {
            _lines.clear();
            auto it = _content.begin();
            auto end = _content.end();

            while (it != end)
            {
                auto begin = it;

                it = std::find(it, end, '\n');

                _lines.emplace_back(begin, it);

                if (it != end) it++;
            }
        }

        void AddCompileOptions(Project* project)
        {
            if (project)
            {
                auto client = project->client();

                if (client) {
                    int i = 0;

                    std::string_view new_opt;

                    while (new_opt = client->GetCompileOptions(i++), new_opt.empty() == false)
                    {
                        _options.push_back(new_opt);
                    }
                }
            }
        }



    public:


        Director(const std::string_view& nm, Project* pro, std::span<std::string_view> ops) :
            _name{ nm },
            _project{ pro },
            _options{ ops.begin(), ops.end() }
        {
            if (_project)
            {
                AddCompileOptions(_project);

                if (_project->IsShared() == false)
                {
                    AddCompileOptions(ProjectManager::instance->GetShared());
                }
            }
        }


        bool CheckCondition(DirectiveRecord& conditions)
        {
            //At a later point this will take an expression of basic numbers and such and try to compile it. No functions, just literals and possible macros.
            //For now there's only one option.

            for (DirectiveRecord& it : conditions.children())

                switch (it.GetDirective().type)
                {
                case DirectiveType::Prefunc:
                    switch (Hash(it.GetTag()))
                    {
                    case "option"_h:
                        if constexpr (1)
                        {
                            auto begin = _options.begin();
                            auto end = _options.end();

                            if (std::find(begin, end, it.GetFront().GetView()) == end)
                                return false;
                        }
                        break;
                    default:
                        report::parse::failure("unknown pre processor function '{}' detected", it.GetView());
                        return false;
                        break;
                    }
                }


            return true;

        }




        bool ConditionalProcess(DirectiveRecord::Iterator it, DirectiveRecord::Iterator end)
        {
            bool enabled = CheckCondition(*it);

            Line start = it->get().GetDirective().line;
            Line finish = 0;

            bool cont = true;

            bool fin = true;

            while (++it != end && cont)
            {

                DirectiveRecord& record = *it;

                if (record.GetDirective().type != DirectiveType::Conditional) {
                    if (!enabled)
                        record.GetDirective().type = DirectiveType::None;

                    continue;
                }

                //All conditionals are disposable regardless.
                record.GetDirective().type = DirectiveType::None;


                switch (Hash(record.GetTag()))
                {
                case "if"_h:
                    fin = ConditionalProcess(it, end);
                    if (!fin) return fin;
                    break;

                case "endif"_h:
                    finish = record.GetDirective().line;
                    cont = false;
                    break;
                }
            }

            if (!finish) {
                report::parse::failure("Un-ended #if detected.");
                return false;
            }

            if (enabled)
            {
                SetLines(false, start);
                SetLines(false, finish);
            }
            else
            {
                SetLines(false, start, finish);
            }
            return fin;
        }


        bool FormatProcess(DirectiveRecord::Iterator it, DirectiveRecord::Iterator end)
        {
            Line start = it->get().GetDirective().line;
            Line finish = 0;

            bool cont = true;

            int fin = 1;

            std::string_view format = it->get().GetView();
            logger::trace("format: {}", format);
            while (++it != end && cont)
            {
                DirectiveRecord& record = *it;

                record.GetDirective().type = DirectiveType::None;


                switch (Hash(record.GetTag()))
                {
                default:
                    ++fin;
                    break;

                case "endformat"_h:
                    if (--fin) {
                        break;
                    }

                    finish = record.GetDirective().line;
                    cont = false;
                    break;
                }
            }

            std::string_view data = GetText(start, finish, true);

            PostProcess post;

            post.data.values["name"] = format;
            post.data.values["text"] = data;
            post.callback = [](Script* script, Director* director, PostProcessData* data)
                {
                    if (auto project = director->_project) {
                        auto& name = data->values["name"];
                        auto& text = data->values["text"];
                        script->AddFormat(name, text);
                        return;

                        if (project->SendFormat(name, text, script) == false)
                            report::compile::warn("Format '{}' failed to be registered to {}.",
                                name, script->GetFullName());
                    }
                };

            this->_postProcesses.push_back(std::move(post));

            if (!finish) {
                report::parse::warn("Un-ended #format detected.");
                return false;
            }

            SetLines(false, start, finish);

            return true;
        }





        bool GeneralProcess(DirectiveRecord& directives)
        {
            auto& dirs = directives.children();

            DirectiveRecord::Iterator it = dirs.begin();
            DirectiveRecord::Iterator end = dirs.end();

            auto _begin = _options.begin();
            auto _end = _options.end();

            bool result = true;

            while (it != end && result)
            {

                DirectiveRecord& record = *it;


                auto old = it;

                auto line = record.GetDirective().line;

                if (IsLineEnabled(line) == true)
                {
                    SetLines(false, record.GetDirective().line);

                    switch (record.GetDirective().type)
                    {
                    case DirectiveType::Requirement:
                        if (CheckCondition(*it) == false) {
                            logger::break_warn("Requirement not met for {} to load.", _name);
                            return false;
                        }
                        break;

                    case DirectiveType::Conditional:
                        result = ConditionalProcess(it, end);
                        break;

                    case DirectiveType::Format:
                        result = FormatProcess(it, end);
                    }
                }

                if (it == old)
                    it++;
            }


            return true;
        }

        void SetContent(const std::string_view& content)
        {
            _content = content;
            Init();
        }

        bool HandlePreprocess(const std::string_view& content, Line line = 1, Column column = 1)
        {
            SetContent(content);
            DirectiveRecord directives;


            PreprocessorParser direct_parse;


            //tmp_directives = Parser::CreateSyntaxTree(std::string{ project->GetName() }, std::string{ name }, contents, &direct_parse);

            if (Parser::CreateSyntaxTree(directives, _content, _name, &direct_parse, line, column) == false) {
                return false;
            }

            if (GeneralProcess(directives) == false)
                return false;

            return true;
        }

        void HandlePostprocess(Script* script)
        {


            for (auto& entry : _options)
            {
                constexpr std::string_view inc = "incremental";

                if (strnicmp(entry.data(), inc.data(), inc.size()) == 0) {
                    script->SetIncremental(true);
                    report::debug("Script {} made incremental.", script->GetName());
                }
            }

            for (auto& post : _postProcesses) {
                post.callback(script, this, &post.data);
            }
        }


        ///////////


        std::string_view GetLine(Line i)
        {
            if (--i == _max)
                return "";

            auto& line = _lines[i];
            return std::string_view{ line.begin, line.end };

        }

        std::string_view GetText(Line start, Line end, bool exclusive = false)
        {
            if (exclusive) {
                start++;
                end--;
            }

            auto first = _lines[start - 1].begin;
            auto last = _lines[end - 1].end;
            return std::string_view{ first, last };
        }

        bool IsLineEnabled(Line i)
        {
            if (--i == _max)
                return "";

            auto& line = _lines[i];
            return line.enabled;

        }


        void SetLines(bool value, Line begin, Line end = _max)
        {
            if (begin == _max)
                return;

            begin--;



            if (end == _max || end == 0)
                end = begin;
            else
                end--;

            if (begin > end) {
                std::swap(begin, end);
            }

            for (; begin <= end; begin++)
            {
                _lines[begin].enabled = value;
            }
        }


        std::string& Clear()
        {
            if (_lines.size() == 0)
                return _content;

            //after using this it becomes invalid.
            for (int i = (int)_lines.size() - 1; i >= 0; i--)
            {
                auto& line = _lines[i];

                if (!line.enabled)
                    _content.erase(line.begin, line.end);
            }
            Init();
            return _content;
        }

        std::string_view name()
        {
            return _name;
        }



        Project* _project = nullptr;


        std::string_view _name;
        std::string _content;
        std::vector<ScriptLine> _lines;
        std::vector<std::string_view> _options;

        std::vector<PostProcess> _postProcesses;


    };
}