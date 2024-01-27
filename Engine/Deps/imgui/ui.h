#ifndef UI_H
#define UI_H
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "misc/fonts/AliTTF.h"              //�����ļ�
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION

#include<mythings/engine/engine.h>
#include<mythings/resource_manager.h>
#include<mythings/engine/state.h>

//����״̬
extern Engine engine;
extern State EngineState;

//data
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

//��ǰ����
void SetFullScreenMode(GLFWwindow* window);
void SetWindowMode(GLFWwindow* window);
void IntSet(string label, int& value, int min, int max, char identifier);
void StepFloatSet(float& value, float min, float max, float& step, char identifier);

//-----------------------------------------------------------------------------------

//imGui��ʼ��
void ImGui_Init(GLFWwindow*& window)
{
    //����ImGui������
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;  // ���� unused �����ľ���
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;//�������
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   //���ü��̿���
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    //������Ϸ�ֱ��ؼ�
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       //���ô���ͣ��
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     //���öര��
    io.MouseDrawCursor = true;  //�������
    io.ConfigViewportsNoAutoMerge = true;

    //����ʼλ��
    //io.WantSetMousePos = true;
    //io.MousePos = ImVec2(0.0f, 0.0f);

    //���ڳ�ʼλ�ô�С
    SetWindowMode(window);

    //����ImGui���----------------------------------------------------------------------
    {
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();
        ImGuiStyle* style = &ImGui::GetStyle();

        //�Զ���ɫ�ʷ�񣨷�ɫ������-----------------------------------------------------------
        //�ı���ɫ
        style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
        style->Colors[ImGuiCol_TextDisabled] = ImColor(188, 138, 218, 155);
        style->Colors[ImGuiCol_TextSelectedBg] = ImColor(188, 138, 218, 155);
        //������ɫ
        style->Colors[ImGuiCol_FrameBg] = ImColor(240, 182, 253, 155);
        style->Colors[ImGuiCol_FrameBgHovered] = ImColor(188, 138, 218, 155);
        style->Colors[ImGuiCol_FrameBgActive] = ImColor(188, 138, 218, 155);
        //��ť��ɫ
        style->Colors[ImGuiCol_Button] = ImColor(188, 138, 218, 255);
        style->Colors[ImGuiCol_ButtonHovered] = ImColor(131, 102, 158, 255);
        style->Colors[ImGuiCol_ButtonActive] = ImColor(200, 160, 200, 255);
        //������ɫ
        style->Colors[ImGuiCol_SliderGrab] = ImColor(230, 200, 245, 255);
        style->Colors[ImGuiCol_SliderGrabActive] = ImColor(200, 160, 200, 255);
        //��������ɫ
        style->Colors[ImGuiCol_ScrollbarBg] = ImColor(131, 102, 158, 255);
        style->Colors[ImGuiCol_ScrollbarGrab] = ImColor(200, 160, 200, 255);
        style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(188, 138, 218, 255);
        style->Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(200, 160, 200, 255);
        //ѡ������ɫ
        style->Colors[ImGuiCol_CheckMark] = ImColor(230, 200, 245, 255);
        //������ɫ
        style->Colors[ImGuiCol_TitleBg] = ImColor(90, 45, 100, 155);
        style->Colors[ImGuiCol_TitleBgActive] = ImColor(188, 138, 218, 155);
        //����ͷ��ɫ
        style->Colors[ImGuiCol_Header] = ImColor(240, 182, 253, 155);
        style->Colors[ImGuiCol_HeaderHovered] = ImColor(188, 138, 218, 155);
        style->Colors[ImGuiCol_HeaderActive] = ImColor(188, 138, 218, 155);
        //���ڱ�ǩ��ɫ
        style->Colors[ImGuiCol_Tab] = ImColor(125, 150, 163, 255);
        style->Colors[ImGuiCol_TabHovered] = ImColor(125, 150, 163, 255);
        style->Colors[ImGuiCol_TabActive] = ImColor(125, 150, 163, 255);
        //������ɫ
        style->Colors[ImGuiCol_WindowBg] = ImColor(219, 166, 230, 155);

        //�Զ���ؼ���ʽ��Բ�������--------------------------------------------------------------
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);      //����Բ����
        ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 12); //������Բ����
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12);    //����Բ����
        ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 12);      //�϶�������
        ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 12);       //��ǩԲ����   
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 12);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12);
    }

    //����ƽ̨/��Ⱦ�����
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    //�ڴ��м�������
    ImFont* Mfont = io.Fonts->AddFontFromMemoryTTF((void*)AliTTF_data, AliTTF_size, 20.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());

}

//imGui����
static float LastX = 0.0f;
static float LastY = 0.0f;
static GLboolean FirstMouse = true;
void GameCameraInput(GLFWwindow*& window)
{
    ImGuiIO& io = ImGui::GetIO();
    (void)io;  // ���� unused �����ľ���

    //����ƶ�
    float xpos = io.MousePos.x;
    float ypos = io.MousePos.y;

    float dx = abs(xpos - LastX);
    float dy = abs(LastY - ypos);
    bool FirstChange = (dx >= 0.1f) || (dy >= 0.1f);
    if (FirstMouse && FirstChange)
    {
        LastX = xpos;
        LastY = ypos;
        FirstMouse = false;
    }
    float xoffset = xpos - LastX;
    float yoffset = LastY - ypos;

    LastX = xpos;
    LastY = ypos;

    EngineState.camera.ProcessMouseMovement(xoffset, yoffset);

    //������
    float MouseRoll = io.MouseWheel;
    EngineState.camera.ProcessMouseScroll(static_cast<float>(MouseRoll));

    //����
    //main��glfw�����Լ�����Ϣ�Ļ�ȡ
}

//����ͣ���ռ�
void DockingSpace(bool& p_open)
{
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", &p_open, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    /*
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Docking"))
        {
            //Disabling fullscreen would allow the window to be moved to the front of other windows,
            //which we can't undo at the moment without finer window depth/z control.
            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

            if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
            if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
            if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
            if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))     dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
            if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
            ImGui::Separator();
            if (ImGui::MenuItem("Close DockSpace", NULL, false, p_open != NULL))
                p_open = false;
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    */

    ImGui::End();
}

//����鿴����
void TextureWatcher()
{
    static bool firstrender3 = true; //�״λ��ƣ��������ô��ڳ�ʼλ�ô�С
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    if (EngineState.IfWatcherOpen)
    {
        ImGui::Begin("TextureWatcher", &EngineState.IfWatcherOpen, window_flags);
        {
            if (firstrender3)
            {
                ImGui::SetWindowPos({ 0,0 });
                ImGui::SetWindowSize({ 500,500 });
                firstrender3 = false;
            }

            // Using a Child allow to fill all the space of the window.
            // It also alows customization
            ImGui::BeginChild("Texture", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysAutoResize);
            // Get the size of the child (i.e. the whole draw size of the windows).
            ImVec2 wsize = ImGui::GetWindowSize();
            // Because I use the texture from OpenGL, I need to invert the V from the UV.

            ImGui::Image((ImTextureID)EngineState.WatchingTexture, wsize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndChild();
        }
        ImGui::End();
    }
}

//ȫ�����Դ���
void EngineStateWindow(GLFWwindow*& window, bool& p_open, ImGuiIO& io)
{
    if (p_open)
    {
        static bool firstrender1 = true; //�״λ��ƣ��������ô��ڳ�ʼλ�ô�С
        {
            ImGui::Begin("EngineState", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
            if (firstrender1)
            {
                ImGui::SetWindowPos({ 850,50 });
                firstrender1 = false;
            }

            //֡����ʾ
            ImGui::TextColored(clear_color, "Application average %.3f ms/frame (%.1f FPS)\n", 1000.0f / io.Framerate, io.Framerate);

            //���ñ�����ɫ
            static float f;
            ImGui::Text(u8"������ɫ");
            ImGui::ColorEdit4("clear color", (float*)&clear_color);

            //ȫ������
            ImGui::Text(u8"��������");
            if (ImGui::Button("FullScreenWindow"))SetFullScreenMode(window); ImGui::SameLine();
            if (ImGui::Button("SimpleWindow"))SetWindowMode(window);

            //��Ⱦģʽ
            ImGui::Text(u8"��Ⱦģʽ"); ImGui::SameLine();
            if (EngineState.RenderMode == Phong)
                ImGui::Text("Phong");
            else if (EngineState.RenderMode == PBR)
                ImGui::Text("PBR");

            if (ImGui::Button("Phong"))
            {
                EngineState.RenderMode = Phong;
                engine.ObjectRenderMode(Phong);
            }
            ImGui::SameLine();
            if (ImGui::Button("PBR"))
            {
                EngineState.RenderMode = PBR;
                engine.ObjectRenderMode(PBR);
            }


            //�߿�ģʽ
            ImGui::Text(u8"����ģʽ");
            if (ImGui::Button("FillModel"))
            {
                EngineState.RenderStyle = FILL_STYLE;
            }
            ImGui::SameLine();
            if (ImGui::Button("LineModel"))
            {
                EngineState.RenderStyle = LINE_STYLE;
            }
            ImGui::SameLine();
            if (ImGui::Button("PointModel"))
            {
                EngineState.RenderStyle = POINT_STYLE;
            }
            //���С����
            static int PointSize = 1;
            ImGui::Text(u8"���С");
            ImGui::SliderInt(u8"����##1", &PointSize, 1, 25);
            glPointSize(PointSize);

            //�߿������
            static int LineWidth = 1;
            ImGui::Text(u8"�߿�");
            ImGui::SliderInt(u8"����##2", &LineWidth, 1, 25);
            glLineWidth(LineWidth);

            //����������
            ImGui::Text(u8"������    "); ImGui::SameLine();
            ImGui::ColorEdit3("##Ambient", glm::value_ptr(EngineState.Ambient));
            ImGui::Text(u8"������ǿ��"); ImGui::SameLine();
            ImGui::SliderFloat("##AmbientIntensity", &EngineState.AmbientIntensity, 0.0f, 1.0f, "%.1f");

            ImGui::End();
        }
    }
}

//����ѡ���
void EngineOptionWindow(bool& p_open, Engine& engine)
{
    static bool firstrender0 = true; //�״λ��ƣ��������ô��ڳ�ʼλ�ô�С
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_AlwaysAutoResize;
    if (p_open)
    {
        ImGui::Begin("SenceSet", NULL, window_flags);
        if (firstrender0)
        {
            ImGui::SetWindowPos({ 50,50 });
            firstrender0 = false;
        }

        //��������
        if (ImGui::CollapsingHeader(u8"��������", ImGuiTreeNodeFlags_None))
        {
            //����ѡ��
            static int selectedIndex = 0;
            int maxObjects = engine.Objects.size();
            Object* selectedObject = engine.Objects.at(selectedIndex);
            IntSet("Selected Index", selectedIndex, 0, maxObjects - 1, 'd');
            ImGui::Text(u8"ѡ������:"); ImGui::SameLine();
            ImGui::PushItemWidth(120);
            ImGui::InputText("##ObjectName", &(engine.Objects.at(selectedIndex)->Name), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll);
            ImGui::PopItemWidth();
            ImGui::Separator();
            //�������
            ImGui::Text(u8"������� "); ImGui::SameLine();
            if (ImGui::Button("Cube"))engine.ObjectAdd(BASIC_CUBE); ImGui::SameLine();
            if (ImGui::Button("Quad"))engine.ObjectAdd(BASIC_QUAD); ImGui::SameLine();
            if (ImGui::Button("Sphere"))engine.ObjectAdd(BASIC_SPHERE); ImGui::SameLine();
            if (ImGui::Button("Model"))engine.ObjectAdd(EXTEND_MODEL);
            ImGui::Separator();

            //�����С
            if (ImGui::CollapsingHeader("Size", ImGuiTreeNodeFlags_None))
            {
                float min_value = 0.0f;
                float max_value = 100.0f;

                ImGui::Text(u8"x�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##x��size��", ImGuiDataType_Float, &(selectedObject->Size.x), &min_value, &max_value);
                static float stepX = 1;
                StepFloatSet(selectedObject->Size.x, min_value, max_value, stepX, 'o');

                ImGui::Text(u8"y�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##y��size��", ImGuiDataType_Float, &(selectedObject->Size.y), &min_value, &max_value);
                static float stepY = 1;
                StepFloatSet(selectedObject->Size.y, min_value, max_value, stepY, 'p');

                ImGui::Text(u8"z�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##z��size��", ImGuiDataType_Float, &(selectedObject->Size.z), &min_value, &max_value);
                static float stepZ = 1;
                StepFloatSet(selectedObject->Size.z, min_value, max_value, stepZ, 'q');
            }

            //����λ��
            if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_None))
            {
                float min_value = -100.0f;
                float max_value = 100.0f;

                ImGui::Text(u8"x�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##x��pos��", ImGuiDataType_Float, &(selectedObject->Position.x), &min_value, &max_value);
                static float stepX = 1;
                StepFloatSet(selectedObject->Position.x, min_value, max_value, stepX, 'x');

                ImGui::Text(u8"y�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##y��pos��", ImGuiDataType_Float, &(selectedObject->Position.y), &min_value, &max_value);
                static float stepY = 1;
                StepFloatSet(selectedObject->Position.y, min_value, max_value, stepY, 'y');

                ImGui::Text(u8"z�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##z��pos��", ImGuiDataType_Float, &(selectedObject->Position.z), &min_value, &max_value);
                static float stepZ = 1;
                StepFloatSet(selectedObject->Position.z, min_value, max_value, stepZ, 'z');
            }
           
            //������ת
            if (ImGui::CollapsingHeader("Rotation", ImGuiTreeNodeFlags_None))
            {
                float min_value = -180.0f;
                float max_value = 180.0f;

                ImGui::Text(u8"x�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##x��rotate��", ImGuiDataType_Float, &(selectedObject->RotateAngle.x), &min_value, &max_value);
                static float stepX = 1;
                StepFloatSet(selectedObject->RotateAngle.x, min_value, max_value, stepX, 'X');

                ImGui::Text(u8"y�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##y��rotate��", ImGuiDataType_Float, &(selectedObject->RotateAngle.y), &min_value, &max_value);
                static float stepY = 1;
                StepFloatSet(selectedObject->RotateAngle.y, min_value, max_value, stepY, 'Y');

                ImGui::Text(u8"z�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##z��rotate��", ImGuiDataType_Float, &(selectedObject->RotateAngle.z), &min_value, &max_value);
                static float stepZ = 1;
                StepFloatSet(selectedObject->RotateAngle.z, min_value, max_value, stepZ, 'Z');
            }
            
            //��������
            if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_None))
            {
                ImGui::Text(u8"MaterialType");
                int value0 = selectedObject->materialType;
                //������������
                if (selectedObject->Class == EXTEND_MODEL)
                {
                    //��ѡ��
                }
                else if (EngineState.RenderMode == Phong)
                {
                    ImGui::RadioButton("PHONG_VALUE##A", &value0, 0); ImGui::SameLine();
                    ImGui::RadioButton("PHONG_MAP##A", &value0, 1);
                }
                else if (EngineState.RenderMode == PBR)
                {
                    ImGui::RadioButton("PBR_VALUE##A", &value0, 2); ImGui::SameLine();
                    ImGui::RadioButton("PBR_MAP##A", &value0, 3);
                }

                switch (value0)
                {
                case 0:selectedObject->materialType = PHONG_VALUE; break;
                case 1:selectedObject->materialType = PHONG_MAP; break;
                case 2:selectedObject->materialType = PBR_VALUE; break;
                case 3:selectedObject->materialType = PBR_MAP; break;
                default:break;
                }
                //���ʾ�����������
                if (EngineState.RenderMode == Phong)
                {
                    if (selectedObject->materialType == PHONG_VALUE)
                    {
                        ImGui::Text("Color"); ImGui::SameLine();
                        ImGui::ColorEdit3("##PHONG_VALUE_COLOR", glm::value_ptr((dynamic_cast<basicObject*>(selectedObject))->Color));
                        ImGui::Text("Specular "); ImGui::SameLine();
                        ImGui::SliderFloat("##PHONG_VALUE_SPEC", &dynamic_cast<basicObject*>(selectedObject)->Specular, 0.0f, 1.0f, "%.1f");
                        ImGui::Text("Shininess"); ImGui::SameLine();
                        ImGui::SliderFloat("##PHONG_VALUE_SHININESS", &dynamic_cast<basicObject*>(selectedObject)->Shininess, 0.0f, 128.0f, "%.1f");
                    }
                    else if (selectedObject->materialType == PHONG_MAP)
                    {
                        ImGui::Text("DiffuseMap"); ImGui::SameLine(); ImGui::Text("SpecularMap");
                        ImTextureID imguiTextureID1 = (ImTextureID)(intptr_t)(dynamic_cast<basicObject*>(selectedObject)->diffuseMap.ID);
                        ImGui::Image(imguiTextureID1, ImVec2(80, 80));
                        if (ImGui::IsItemClicked())
                        {
                            EngineState.WatchingTexture = dynamic_cast<basicObject*>(selectedObject)->diffuseMap.ID;
                            EngineState.IfWatcherOpen = true;
                        }
                        ImGui::SameLine();
                        ImTextureID imguiTextureID2 = (ImTextureID)(intptr_t)(dynamic_cast<basicObject*>(selectedObject)->specularMap.ID);
                        ImGui::Image(imguiTextureID2, ImVec2(80, 80));
                        if (ImGui::IsItemClicked())
                        {
                            EngineState.WatchingTexture = dynamic_cast<basicObject*>(selectedObject)->specularMap.ID;
                            EngineState.IfWatcherOpen = true;
                        }

                        ImGui::Text("Shininess"); ImGui::SameLine();
                        ImGui::SliderFloat("##PHONG_MAP_SHININESS", &dynamic_cast<basicObject*>(selectedObject)->Shininess, 0.0f, 128.0f, "%.1f");
                    }
                    else if (selectedObject->materialType == MODEL_PHONG)
                    {
                        ImGui::Text("Shininess"); ImGui::SameLine();
                        ImGui::SliderFloat("##MODEL_SHININESS", &dynamic_cast<modelObject*>(selectedObject)->Shininess, 0.0f, 128.0f, "%.1f");
                    }
                }
                else if (EngineState.RenderMode == PBR)
                {
                    if (selectedObject->materialType == PBR_VALUE)
                    {
                        ImGui::Text("Albedo"); ImGui::SameLine();
                        ImGui::ColorEdit3("##PBR_VALUE_ALBEDO", glm::value_ptr((dynamic_cast<basicObject*>(selectedObject))->albedo));
                        ImGui::Text("Metallic "); ImGui::SameLine();
                        ImGui::SliderFloat("##PBR_VALUE_METALLIC", &dynamic_cast<basicObject*>(selectedObject)->metallic, 0.0f, 1.0f, "%.1f");
                        ImGui::Text("Roughness"); ImGui::SameLine();
                        ImGui::SliderFloat("##PBR_VALUE_ROUGHNESS", &dynamic_cast<basicObject*>(selectedObject)->roughness, 0.0f, 1.0f, "%.1f");
                        ImGui::Text("Ao       "); ImGui::SameLine();
                        ImGui::SliderFloat("##PBR_VALUE_AO", &dynamic_cast<basicObject*>(selectedObject)->ao, 0.0f, 1.0f, "%.1f");
                    }
                    else if (selectedObject->materialType == PBR_MAP)
                    {
                        ImGui::Text("AlbedoMap"); ImGui::SameLine();
                        ImGui::Text("NormalMap"); ImGui::SameLine();
                        ImGui::Text("MetallicMap");

                        ImTextureID imguiTextureID3 = (ImTextureID)(intptr_t)(dynamic_cast<basicObject*>(selectedObject)->albedoMap.ID);
                        ImGui::Image(imguiTextureID3, ImVec2(80, 80));
                        if (ImGui::IsItemClicked())
                        {
                            EngineState.WatchingTexture = dynamic_cast<basicObject*>(selectedObject)->albedoMap.ID;
                            EngineState.IfWatcherOpen = true;
                        }
                        ImGui::SameLine();

                        ImTextureID imguiTextureID4 = (ImTextureID)(intptr_t)(dynamic_cast<basicObject*>(selectedObject)->normalMap.ID);
                        ImGui::Image(imguiTextureID4, ImVec2(80, 80));
                        if (ImGui::IsItemClicked())
                        {
                            EngineState.WatchingTexture = dynamic_cast<basicObject*>(selectedObject)->normalMap.ID;
                            EngineState.IfWatcherOpen = true;
                        }
                        ImGui::SameLine();

                        ImTextureID imguiTextureID5 = (ImTextureID)(intptr_t)(dynamic_cast<basicObject*>(selectedObject)->metallicMap.ID);
                        ImGui::Image(imguiTextureID5, ImVec2(80, 80));
                        if (ImGui::IsItemClicked())
                        {
                            EngineState.WatchingTexture = dynamic_cast<basicObject*>(selectedObject)->metallicMap.ID;
                            EngineState.IfWatcherOpen = true;
                        }

                        ImGui::Text("RoughnessMap"); ImGui::SameLine();
                        ImGui::Text("AoMap");
                        ImTextureID imguiTextureID6 = (ImTextureID)(intptr_t)(dynamic_cast<basicObject*>(selectedObject)->roughnessMap.ID);
                        ImGui::Image(imguiTextureID6, ImVec2(80, 80));
                        if (ImGui::IsItemClicked())
                        {
                            EngineState.WatchingTexture = dynamic_cast<basicObject*>(selectedObject)->roughnessMap.ID;
                            EngineState.IfWatcherOpen = true;
                        }
                        ImGui::SameLine();
                        ImTextureID imguiTextureID7 = (ImTextureID)(intptr_t)(dynamic_cast<basicObject*>(selectedObject)->aoMap.ID);
                        ImGui::Image(imguiTextureID7, ImVec2(80, 80));
                        if (ImGui::IsItemClicked())
                        {
                            EngineState.WatchingTexture = dynamic_cast<basicObject*>(selectedObject)->aoMap.ID;
                            EngineState.IfWatcherOpen = true;
                        }

                    }
                    else if (selectedObject->materialType == MODEL_PBR)
                    {
                        ImGui::Text("Metallic "); ImGui::SameLine();
                        ImGui::SliderFloat("##MODEL_METALLIC", &dynamic_cast<modelObject*>(selectedObject)->metallic, 0.0f, 1.0f, "%.1f");
                        ImGui::Text("Roughness"); ImGui::SameLine();
                        ImGui::SliderFloat("##MODEL_ROUGHNESS", &dynamic_cast<modelObject*>(selectedObject)->roughness, 0.0f, 1.0f, "%.1f");
                        ImGui::Text("Ao       "); ImGui::SameLine();
                        ImGui::SliderFloat("##MODEL_AO", &dynamic_cast<modelObject*>(selectedObject)->ao, 0.0f, 1.0f, "%.1f");
                    }
                }
            }
        }
        ImGui::Separator();
        ImGui::Separator();

        //��Դ����
        if (ImGui::CollapsingHeader(u8"��Դ����", ImGuiTreeNodeFlags_None))
        {
            //��Դѡ��
            static int selectedIndex_Light = 0;
            int maxLights = engine.Lights.size();
            PointLight* selectedLight = dynamic_cast<PointLight*>(engine.Lights.at(selectedIndex_Light));
            IntSet("Selected Index", selectedIndex_Light, 0, maxLights - 1, 'D');
            ImGui::Text(u8"ѡ�й�Դ:"); ImGui::SameLine();
            ImGui::PushItemWidth(120);
            ImGui::InputText("##LightName", &(engine.Lights.at(selectedIndex_Light)->Name), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll);
            ImGui::PopItemWidth();
            ImGui::Separator();
            //��Դ���
            ImGui::Text(u8"��ӹ�Դ "); ImGui::SameLine();
            if (ImGui::Button("Point##LIGHT"))engine.LightAdd(POINT_LIGHT); ImGui::SameLine();
            if (ImGui::Button("Directional##LIGHT"))engine.LightAdd(DIR_LIGHT); ImGui::SameLine();
            if (ImGui::Button("Spot##LIGHT"))engine.LightAdd(SPOT_LIGHT);
            ImGui::Separator();

            //�����С
            if (ImGui::CollapsingHeader("Size##LIGHT", ImGuiTreeNodeFlags_None))
            {
                float min_value = 0.0f;
                float max_value = 1.0f;

                ImGui::Text(u8"x�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##x��size��##LIGHT", ImGuiDataType_Float, &(selectedLight->Size.x), &min_value, &max_value);
                static float stepX = 0.1f;
                StepFloatSet(selectedLight->Size.x, min_value, max_value, stepX, 'O');

                ImGui::Text(u8"y�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##y��size��##LIGHT", ImGuiDataType_Float, &(selectedLight->Size.y), &min_value, &max_value);
                static float stepY = 0.1f;
                StepFloatSet(selectedLight->Size.y, min_value, max_value, stepY, 'P');

                ImGui::Text(u8"z�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##z��size��##LIGHT", ImGuiDataType_Float, &(selectedLight->Size.z), &min_value, &max_value);
                static float stepZ = 0.1f;
                StepFloatSet(selectedLight->Size.z, min_value, max_value, stepZ, 'Q');
            }

            //��Դλ��
            if (ImGui::CollapsingHeader("Position##LIGHT", ImGuiTreeNodeFlags_None))
            {
                float min_value = -100.0f;
                float max_value = 100.0f;

                ImGui::Text(u8"x�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##x��pos��##LIGHT", ImGuiDataType_Float, &(selectedLight->Position.x), &min_value, &max_value);
                static float stepX = 1;
                StepFloatSet(selectedLight->Position.x, min_value, max_value, stepX, 'a');

                ImGui::Text(u8"y�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##y��pos��##LIGHT", ImGuiDataType_Float, &(selectedLight->Position.y), &min_value, &max_value);
                static float stepY = 1;
                StepFloatSet(selectedLight->Position.y, min_value, max_value, stepY, 'b');

                ImGui::Text(u8"z�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##z��pos��##LIGHT", ImGuiDataType_Float, &(selectedLight->Position.z), &min_value, &max_value);
                static float stepZ = 1;
                StepFloatSet(selectedLight->Position.z, min_value, max_value, stepZ, 'c');
            }
            
            //��Դ��ת
            if (ImGui::CollapsingHeader("Rotation##LIGHT", ImGuiTreeNodeFlags_None))
            {
                float min_value = -180.0f;
                float max_value = 180.0f;

                ImGui::Text(u8"x�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##x��rotate��##LIGHT", ImGuiDataType_Float, &(selectedLight->RotateAngle.x), &min_value, &max_value);
                static float stepX = 1;
                StepFloatSet(selectedLight->RotateAngle.x, min_value, max_value, stepX, 'A');

                ImGui::Text(u8"y�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##y��rotate��##LIGHT", ImGuiDataType_Float, &(selectedLight->RotateAngle.y), &min_value, &max_value);
                static float stepY = 1;
                StepFloatSet(selectedLight->RotateAngle.y, min_value, max_value, stepY, 'B');

                ImGui::Text(u8"z�᣺");
                ImGui::SameLine();
                ImGui::SliderScalar(u8"##z��rotate��##LIGHT", ImGuiDataType_Float, &(selectedLight->RotateAngle.z), &min_value, &max_value);
                static float stepZ = 1;
                StepFloatSet(selectedLight->RotateAngle.z, min_value, max_value, stepZ, 'C');
            }
            
            //��Դ��ɫ
            ImGui::Text("Color"); ImGui::SameLine();
            ImGui::ColorEdit3("##LIGHT_COLOR", glm::value_ptr((selectedLight)->Color));
        }
        ImGui::Separator();
        ImGui::Separator();

        ImGui::End();
    }
}

//Game��������
void GameSenceWindow(GLFWwindow*& window, unsigned int& texture)
{
    static bool firstrender2 = true; //�״λ��ƣ��������ô��ڳ�ʼλ�ô�С
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove;

    ImGui::Begin("GameWindow", (bool*)0, window_flags);
    {
        if (firstrender2)
        {
            ImGui::SetWindowPos({ 514,288 });
            ImGui::SetWindowSize({ 768,432 });
            firstrender2 = false;
        }

        //��game���ڱ��۽��������������
        bool is_focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
        if (is_focused)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);        //�������߽�ģ��
            GameCameraInput(window);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);          //�������߽��ƶ�
            FirstMouse = true;
        }

        // Using a Child allow to fill all the space of the window.
        // It also alows customization
        ImGui::BeginChild("GameRender", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysAutoResize);
        // Get the size of the child (i.e. the whole draw size of the windows).
        ImVec2 wsize = ImGui::GetWindowSize();
        // Because I use the texture from OpenGL, I need to invert the V from the UV.

        ImGui::Image((ImTextureID)texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndChild();
    }
    ImGui::End();
}

//imGui����
void ImGui_Render(GLFWwindow*& window, unsigned int texture, bool& BockingSpace_open, Engine& engine)
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //std::cout << "(" << io.MousePos.x << "," << io.MousePos.y << ")" << std::endl;

    //��ʼImGui֡
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //���ɸ�����
    static bool OptWin_open = true;
    static bool DataWin_open = true;
    if (BockingSpace_open)
    {
        DockingSpace(BockingSpace_open);

        EngineOptionWindow(OptWin_open, engine);

        EngineStateWindow(window, DataWin_open, io);

        GameSenceWindow(window, texture);

        TextureWatcher();
    }

    //����UI
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

//__ȫ��ģʽ
void SetFullScreenMode(GLFWwindow* window)
{
    //��ȡ��ʾ������
    int monitorCount;
    //GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
    GLFWmonitor** pMonitor = glfwGetMonitors(&monitorCount);
    //std::cout << "Now, Screen number is " << monitorCount << std::endl;

    //��ȡ��ʾ����ߣ����أ�
    int screen_width, screen_height;
    for (int i = 0; i < monitorCount; i++)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(pMonitor[i]);
        //std::cout << "Screen size is X = " << mode->width << ", Y = " << mode->height << std::endl;
        screen_width = mode->width;
        screen_height = mode->height;
    }

    // ���ô��ڵ�ȫ��״̬  
    glfwSetWindowMonitor(window, pMonitor[0], 0, 0, screen_width, screen_height, 120);

}

//__����ģʽ
void SetWindowMode(GLFWwindow* window)
{
    // ���ô��ڵĿ�Ⱥ͸߶�  
    int window_width = 1680;
    int window_height = 920;
    glfwSetWindowSize(window, window_width, window_height);
    // ���ô�������Ļ�ϵ�λ��  
    int screen_width = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
    int screen_height = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
    glfwSetWindowPos(window, (screen_width - window_width) / 2, (screen_height - window_height) / 2);

    EngineState.SCR_WIDTH = window_width;
    EngineState.SCR_HEIGHT = window_height;

    glfwSetWindowMonitor(window, NULL, 100, 100, window_width, window_height, 0);
}

//���ƿؼ�----------
//identifierΪ�ؼ���ǣ���Ϊimgui�Ĳ�ͬ�ؼ�������ͬʱ����ֻ���
void IntSet(string label, int& value, int min, int max, char identifier)
{
    std::string input = std::to_string(value);

    ImGui::Text("%s", label.c_str());
    ImGui::SameLine();

    ImGui::PushItemWidth(100);
    if (ImGui::InputText(("##InputField0" + std::string(1, identifier)).c_str(), &input, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll))
    {
        try {
            value = std::stoi(input);

            // Ensure input is within the valid range
            value = std::max(min, std::min(value, max));
        }
        catch (const std::invalid_argument& e) {
            // Handle invalid argument (e.g., if input is not a valid integer)
            // You might want to display an error message or handle it in an appropriate way.
            // std::cerr << "Invalid argument: " << e.what() << std::endl;
        }
        catch (const std::out_of_range& e) {
            // Handle out of range (e.g., if input is too large or too small for int)
            // std::cerr << "Out of range: " << e.what() << std::endl;
        }
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (ImGui::Button(("-##0" + std::string(1, identifier)).c_str()) && value > min)
        value--;

    ImGui::SameLine();
    if (ImGui::Button(("+##0" + std::string(1, identifier)).c_str()) && value < max)
        value++;
}

void StepFloatSet(float& value, float min, float max, float& step, char identifier)
{
    // ���������
    ImGui::Text("Step:");
    ImGui::SameLine();

    std::string stepStr = std::to_string(step);
    ImGui::PushItemWidth(80);
    ImGui::InputText(("##InputField1" + std::string(1, identifier)).c_str(), &stepStr, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll);
    ImGui::PopItemWidth();
    step = std::stof(stepStr);

    // ȷ��ֵ�ںϷ���Χ��
    value = std::max(static_cast<float>(min), std::min(value, static_cast<float>(max)));

    // ������ť
    ImGui::SameLine();
    if (ImGui::Button(("-##1" + std::string(1, identifier)).c_str()) && value - step > min)
        value -= step;
    ImGui::SameLine();
    if (ImGui::Button(("+##1" + std::string(1, identifier)).c_str()) && value + step < max)
        value += step;
}

#endif
