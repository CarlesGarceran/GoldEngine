#pragma once

bool showFbxConverter = false;
std::string fbxFile = "";
std::string exportId = "";
std::string exportDesc = "";
AssimpConverter* fbxConverter = nullptr;

// EXTERN \\

extern String^ GetAccessRoute(Engine::Internal::Components::GameObject^ object);
extern bool selectionLock;
extern msclr::gcroot<Engine::Internal::Components::GameObject^> selectionObject;

/// PROPERTY EDITOR - ATTRIBUTES \\\

void DoubleEditor(Engine::Scripting::Attribute^ attrib)
{
	double tmp = (double)attrib->getValue();
	double step = 1.0;
	double step_fast = 5.0;

	if (ImGui::InputScalar(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), ImGuiDataType_Double, &tmp, &step, &step_fast, "%.3f"))
	{
		attrib->setValue(tmp, false);
		attrib->setType(double::typeid);
	}
}

void Vector3Editor(Engine::Scripting::Attribute^ attrib)
{
	Engine::Components::Vector3 vector = attrib->getValue<Engine::Components::Vector3>();

	float data[3] = { vector.x, vector.y, vector.z };

	if (ImGui::DragFloat3(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), data, 1.0f, -INFINITY, INFINITY, "%.3f"))
	{
		attrib->setValue(Engine::Components::Vector3::create(data));
	}
}

void Vector2Editor(Engine::Scripting::Attribute^ attrib)
{
	Engine::Components::Vector2 vector = (Engine::Components::Vector2)attrib->getValue();

	float data[2] = { vector.x, vector.y };

	if (ImGui::DragFloat2(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), data, 1.0f, -INFINITY, INFINITY, "%.3f"))
	{
		attrib->setValue(Engine::Components::Vector2::create(data));
	}
}

void BoolEditor(Engine::Scripting::Attribute^ attrib)
{
	bool tmp = (bool)attrib->getValue();

	bool value = (bool)tmp;

	if (ImGui::Checkbox(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), &value))
	{
		attrib->setValue(value, false);
		attrib->setType(bool::typeid);
	}
}

void StringEditor(Engine::Scripting::Attribute^ attrib)
{
	if (attrib->getValueType() != String::typeid)
		return;

	String^ value = (String^)attrib->getValue();

	if (value == nullptr)
		value = "";

	std::string str = CastStringToNative(value);

	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 25);
	if (ImGui::InputText(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), &str))
	{
		attrib->setValue(gcnew String(str.c_str()));
	}
}

void ColorEditor(Engine::Scripting::Attribute^ attrib) 
{
	Engine::Components::Color^ value = nullptr;

	if (attrib->getValue()->GetType() == Newtonsoft::Json::Linq::JObject::typeid)
	{
		auto v = (Newtonsoft::Json::Linq::JObject^)attrib->getValue();
		value = v->ToObject<Engine::Components::Color^>();
	}
	else
	{
		value = (Engine::Components::Color^)attrib->getValue();
	}

	auto float4 = ImGui::ColorConvertU32ToFloat4(ImU32(value->toRGBA()));

	float rawData[4] =
	{
		float4.x,
		float4.y,
		float4.z,
		float4.w
	};

	if (ImGui::ColorEdit4(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), rawData))
	{
		attrib->setValue(gcnew Engine::Components::Color(ImGui::ColorConvertFloat4ToU32(ImVec4(rawData[0], rawData[1], rawData[2], rawData[3]))), false);
	}
}

void IntegerEditor(Engine::Scripting::Attribute^ attrib) 
{
	int value = (int)attrib->getValue();

	if (ImGui::InputInt(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), &value, 1, 1))
	{
		attrib->setValue(gcnew Int32(value), false);
		attrib->setType(Int32::typeid);
	}
}

void EnumEditor(Engine::Scripting::Attribute^ attrib)
{
	System::Enum^ enumerator = attrib->getValue<System::Enum^>();
	auto underlyingType = enumerator->GetType();
	auto enumNames = enumerator->GetNames(underlyingType);

	if (ImGui::BeginCombo(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), CastStringToNative(enumerator->ToString()).c_str()))
	{
		for (int x = 0; x < enumNames->Length; x++)
		{
			auto name = enumNames[x];
			bool isSame = (bool)name->Equals(enumerator->ToString());
			if (ImGui::Selectable(CastStringToNative(name).c_str(), &isSame))
			{
				attrib->setValue(enumerator->ToObject(attrib->getCurrentType(), x), false);
			}
		}

		ImGui::EndCombo();
	}
}

void LongEditor(Engine::Scripting::Attribute^ attrib)
{
	long value = (Int64)attrib->getValue();
	long step = 1;

	if (ImGui::InputScalar(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), ImGuiDataType_S64, &value, &step, &step))
	{
		attrib->setValue(gcnew Int64(value), false);
		attrib->setType(Int64::typeid);
	}
}

void FloatEditor(Engine::Scripting::Attribute^ attrib)
{
	float tmp = (float)attrib->getValue();

	float value = (float)tmp;

	if (ImGui::InputFloat(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), &value, 0.1f, 0.5f, "%.3f"))
	{
		attrib->setValue(value, false);
		attrib->setType(float::typeid);
	}
}

void SingleEditor(Engine::Scripting::Attribute^ attrib)
{
	float tmp = (float)attrib->getValue();

	float value = (float)tmp;

	if (ImGui::InputFloat(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), &value, 0.1f, 0.5f, "%.3f"))
	{
		attrib->setValue(value, false);
		attrib->setType(float::typeid);
	}
}

void UnsignedIntEditor(Engine::Scripting::Attribute^ attrib)
{
	unsigned int value = (unsigned int)attrib->getValue();
	unsigned int tmp = value;
	unsigned int step = 1;

	if (ImGui::InputScalar(CastStringToNative("###PROPERTY_EDITOR_##" + attrib->name).c_str(), ImGuiDataType_U32, &tmp, &step, &step, "%d"))
	{
		attrib->setValue(gcnew UInt32(tmp), false);
		attrib->setType(UInt32::typeid);
	}
}

void ListEditor(Engine::Scripting::Attribute^ attrib)
{
	System::Collections::Generic::List<Object^>^ list = attrib->getValueAs< System::Collections::Generic::List<Object^>^>();
}


/// PROPERTY EDITOR - REFLECTION - PROPERTIES \\\

void LongEditor(System::Object^ obj, System::Reflection::PropertyInfo^ fieldInfo)
{
	long value = (Int64)fieldInfo->GetValue(obj);
	long step = 1;

	if (ImGui::InputScalar(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), ImGuiDataType_S64, &value, &step, &step))
	{
		fieldInfo->SetValue(obj, gcnew Int64(value));
	}
}

void DoubleEditor(System::Object^ obj, System::Reflection::PropertyInfo^ fieldInfo)
{
	double value = (double)fieldInfo->GetValue(obj);
	double step = 1.0;
	double step_fast = 5.0;

	if (ImGui::InputScalar(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), ImGuiDataType_Double, &value, &step, &step_fast, "%.3f"))
	{
		fieldInfo->SetValue(obj, value);
	}
}

void Vector3Editor(System::Object^ obj, System::Reflection::PropertyInfo^ fieldInfo)
{
	Engine::Components::Vector3 vector = (Engine::Components::Vector3)fieldInfo->GetValue(obj);

	float data[3] = { vector.x, vector.y, vector.z };

	if (ImGui::DragFloat3(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), data, 1.0f, -INFINITY, INFINITY, "%.3f"))
	{
		fieldInfo->SetValue(obj, Engine::Components::Vector3::create(data));
	}
}

void Vector2Editor(System::Object^ obj, System::Reflection::PropertyInfo^ fieldInfo)
{
	Engine::Components::Vector2 vector = (Engine::Components::Vector2)fieldInfo->GetValue(obj);

	float data[2] = { vector.x, vector.y };

	if (ImGui::DragFloat2(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), data, 1.0f, -INFINITY, INFINITY, "%.3f"))
	{
		fieldInfo->SetValue(obj, Engine::Components::Vector2::create(data));
	}
}

void IntegerEditor(System::Object^ obj, System::Reflection::PropertyInfo^ fieldInfo)
{
	int value = (int)fieldInfo->GetValue(obj);

	if (ImGui::InputInt(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), &value, 1, 1))
	{
		fieldInfo->SetValue(obj, gcnew Int32(value));
	}
}

void FloatEditor(System::Object^ obj, System::Reflection::PropertyInfo^ fieldInfo)
{
	float tmp = (float)fieldInfo->GetValue(obj);
	float value = (float)tmp;

	if (ImGui::InputFloat(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), &value, 0.1f, 0.5f, "%.3f"))
	{
		fieldInfo->SetValue(obj, value);
	}
}

void EnumEditor(System::Object^ obj, System::Reflection::PropertyInfo^ fieldInfo)
{
	System::Enum^ enumerator = (System::Enum^)fieldInfo->GetValue(obj);
	auto underlyingType = enumerator->GetType();
	auto enumNames = enumerator->GetNames(underlyingType);

	if (ImGui::BeginCombo(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), CastStringToNative(enumerator->ToString()).c_str()))
	{
		for (int x = 0; x < enumNames->Length; x++)
		{
			auto name = enumNames[x];
			bool isSame = (bool)name->Equals(enumerator->ToString());
			if (ImGui::Selectable(CastStringToNative(name).c_str(), &isSame))
			{
				fieldInfo->SetValue(obj, enumerator->ToObject(fieldInfo->PropertyType, x));
			}
		}

		ImGui::EndCombo();
	}
}

void UnsignedIntEditor(System::Object^ obj, System::Reflection::PropertyInfo^ fieldInfo)
{
	unsigned int value = (unsigned int)fieldInfo->GetValue(obj);
	unsigned int tmp = value;
	unsigned int step = 1;

	if (ImGui::InputScalar(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), ImGuiDataType_U32, &tmp, &step, &step, "%d", ImGuiInputTextFlags_EnterReturnsTrue))
	{
		fieldInfo->SetValue(obj, gcnew UInt32(tmp));
	}
}

void BoolEditor(System::Object^ obj, System::Reflection::PropertyInfo^ fieldInfo)
{
	bool tmp = (bool)fieldInfo->GetValue(obj);

	bool value = (bool)tmp;

	if (ImGui::Checkbox(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), &value))
	{
		fieldInfo->SetValue(obj, value);
	}
}

void StringEditor(System::Object^ obj, System::Reflection::PropertyInfo^ fieldInfo)
{
	String^ value = (String^)fieldInfo->GetValue(obj);

	if (value == nullptr)
		value = "";

	std::string str = CastStringToNative(value);

	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 25);
	if (ImGui::InputText(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), &str))
	{
		fieldInfo->SetValue(obj, gcnew String(str.c_str()));
	}
}

void ColorEditor(System::Object^ obj, System::Reflection::PropertyInfo^ fieldInfo)
{
	Engine::Components::Color^ value = (Engine::Components::Color^)fieldInfo->GetValue(obj);

	auto float4 = ImGui::ColorConvertU32ToFloat4(ImU32(value->toRGBA()));

	float rawData[4] =
	{
		float4.x,
		float4.y,
		float4.z,
		float4.w
	};

	if (ImGui::ColorEdit4(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), rawData))
	{
		unsigned int hex = ImGui::ColorConvertFloat4ToU32(ImVec4(rawData[0], rawData[1], rawData[2], rawData[3]));
		fieldInfo->SetValue(obj, gcnew Engine::Components::Color(hex));
	}
}

void GameObjectEditor(System::Object^ obj, System::Reflection::PropertyInfo^ fieldInfo)
{
	std::string temp = std::string("");
	if (fieldInfo->GetValue(obj) == nullptr)
	{
		temp = CastStringToNative("NOT ASSIGNED - (NULL)###" + fieldInfo->Name);
	}
	else
	{
		Engine::Internal::Components::GameObject^ value = (Engine::Internal::Components::GameObject^)fieldInfo->GetValue(obj);

		temp = CastStringToNative(value->name + " - (" + GetAccessRoute(value) + ")###" + fieldInfo->Name);
	}

	if (ImGui::Button(temp.c_str()))
	{
		if (!selectionLock)
		{
			selectionLock = true;
		}
		else
		{
			if (selectionObject->GetType() == fieldInfo->PropertyType || fieldInfo->PropertyType->IsAssignableFrom(selectionObject->GetType()))
			{
				fieldInfo->SetValue(obj, selectionObject);
			}
			else
			{
				fieldInfo->SetValue(obj, nullptr);
			}

			selectionLock = false;
		}
	}
}


void InstanceReferenceEditor(System::Object^ obj, System::Reflection::PropertyInfo^ fieldInfo)
{
	Type^ fieldType = fieldInfo->PropertyType;
	Type^ innerType = fieldType->GetGenericArguments()[0];
	Object^ boxedValue = fieldInfo->GetValue(obj);

	System::Reflection::PropertyInfo^ instanceProp = fieldType->GetProperty("Instance");

	Object^ currentInstance = nullptr;
	if (boxedValue != nullptr)
		currentInstance = instanceProp->GetValue(boxedValue);

	std::string label;
	if (boxedValue == nullptr || currentInstance == nullptr)
	{
		label = CastStringToNative(
			"NOT ASSIGNED - (NULL)###" + fieldInfo->Name);
	}
	else
	{
		Engine::Internal::Components::GameObject^ go =
			safe_cast<Engine::Internal::Components::GameObject^>(currentInstance);

		label = CastStringToNative(
			go->name + " - (" + GetAccessRoute(go) + ")###" + fieldInfo->Name);
	}

	if (ImGui::Button(label.c_str()))
	{
		if (!selectionLock)
		{
			selectionLock = true;
		}
		else
		{
			Engine::Internal::Components::GameObject^ selectedObject = selectionObject;

			bool assignable = selectedObject != nullptr &&
				(innerType->IsAssignableFrom(selectedObject->GetType()));

			instanceProp->SetValue(
				boxedValue,
				assignable ? selectedObject : nullptr
			);

			fieldInfo->SetValue(obj, boxedValue);

			selectionLock = false;
		}
	}
}

/// PROPERTY EDITOR - REFLECTION - FIELDS \\\

void LongEditor(System::Object^ obj, System::Reflection::FieldInfo^ fieldInfo)
{
	long value = (Int64)fieldInfo->GetValue(obj);
	long step = 1;

	if (ImGui::InputScalar(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), ImGuiDataType_S64, &value, &step, &step))
	{
		fieldInfo->SetValue(obj, gcnew Int64(value));
	}
}

void DoubleEditor(System::Object^ obj, System::Reflection::FieldInfo^ fieldInfo)
{
	double value = (double)fieldInfo->GetValue(obj);
	double step = 1.0;
	double step_fast = 5.0;
	
	if (ImGui::InputScalar(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), ImGuiDataType_Double, &value, &step, &step_fast, "%.3f"))
	{
		fieldInfo->SetValue(obj, value);
	}
}

void Vector3Editor(System::Object^ obj, System::Reflection::FieldInfo^ fieldInfo)
{
	Engine::Components::Vector3 vector = (Engine::Components::Vector3)fieldInfo->GetValue(obj);

	float data[3] = { vector.x, vector.y, vector.z };

	if (ImGui::DragFloat3(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), data, 1.0f, -INFINITY, INFINITY, "%.3f"))
	{
		fieldInfo->SetValue(obj, Engine::Components::Vector3::create(data));
	}
}

void Vector2Editor(System::Object^ obj, System::Reflection::FieldInfo^ fieldInfo)
{
	Engine::Components::Vector2 vector = (Engine::Components::Vector2)fieldInfo->GetValue(obj);

	float data[2] = { vector.x, vector.y };

	if (ImGui::DragFloat2(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), data, 1.0f, -INFINITY, INFINITY, "%.3f"))
	{
		fieldInfo->SetValue(obj, Engine::Components::Vector2::create(data));
	}
}

void IntegerEditor(System::Object^ obj, System::Reflection::FieldInfo^ fieldInfo)
{
	int value = (int)fieldInfo->GetValue(obj);

	if (ImGui::InputInt(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), &value, 1, 1))
	{
		fieldInfo->SetValue(obj, gcnew Int32(value));
	}
}

void FloatEditor(System::Object^ obj, System::Reflection::FieldInfo^ fieldInfo)
{
	float tmp = (float)fieldInfo->GetValue(obj);
	float value = (float)tmp;

	if (ImGui::InputFloat(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), &value, 0.1f, 0.5f, "%.3f"))
	{
		fieldInfo->SetValue(obj, value);
	}
}

void EnumEditor(System::Object^ obj, System::Reflection::FieldInfo^ fieldInfo)
{
	System::Enum^ enumerator = (System::Enum^)fieldInfo->GetValue(obj);
	auto underlyingType = enumerator->GetType();
	auto enumNames = enumerator->GetNames(underlyingType);

	if (ImGui::BeginCombo(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), CastStringToNative(enumerator->ToString()).c_str()))
	{
		for (int x = 0; x < enumNames->Length; x++)
		{
			auto name = enumNames[x];
			bool isSame = (bool)name->Equals(enumerator->ToString());
			if (ImGui::Selectable(CastStringToNative(name).c_str(), &isSame))
			{
				fieldInfo->SetValue(obj, enumerator->ToObject(fieldInfo->FieldType, x));
			}
		}

		ImGui::EndCombo();
	}
}

void UnsignedIntEditor(System::Object^ obj, System::Reflection::FieldInfo^ fieldInfo)
{
	unsigned int value = (unsigned int)fieldInfo->GetValue(obj);
	unsigned int tmp = value;
	unsigned int step = 1;

	if (ImGui::InputScalar(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), ImGuiDataType_U32, &tmp, &step, &step, "%d", ImGuiInputTextFlags_EnterReturnsTrue))
	{
		fieldInfo->SetValue(obj, gcnew UInt32(tmp));
	}
}

void BoolEditor(System::Object^ obj, System::Reflection::FieldInfo^ fieldInfo)
{
	bool tmp = (bool)fieldInfo->GetValue(obj);

	bool value = (bool)tmp;

	if (ImGui::Checkbox(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), &value))
	{
		fieldInfo->SetValue(obj, value);
	}
}

void StringEditor(System::Object^ obj, System::Reflection::FieldInfo^ fieldInfo)
{
	String^ value = (String^)fieldInfo->GetValue(obj);

	if (value == nullptr)
		value = "";

	std::string str = CastStringToNative(value);

	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 25);
	if (ImGui::InputText(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), &str))
	{
		fieldInfo->SetValue(obj, gcnew String(str.c_str()));
	}
}

void ColorEditor(System::Object^ obj, System::Reflection::FieldInfo^ fieldInfo)
{
	Engine::Components::Color^ value = (Engine::Components::Color^)fieldInfo->GetValue(obj);

	auto float4 = ImGui::ColorConvertU32ToFloat4(ImU32(value->toRGBA()));

	float rawData[4] =
	{
		float4.x,
		float4.y,
		float4.z,
		float4.w
	};

	if (ImGui::ColorEdit4(CastStringToNative("###PROPERTY_EDITOR_##" + fieldInfo->Name).c_str(), rawData))
	{
		unsigned int hex = ImGui::ColorConvertFloat4ToU32(ImVec4(rawData[0], rawData[1], rawData[2], rawData[3]));
		fieldInfo->SetValue(obj, gcnew Engine::Components::Color(hex));
	}
}

void GameObjectEditor(System::Object^ obj, System::Reflection::FieldInfo^ fieldInfo)
{
	std::string temp = std::string("");
	if (fieldInfo->GetValue(obj) == nullptr)
	{
		temp = CastStringToNative("NOT ASSIGNED - (NULL)###" + fieldInfo->Name);
	}
	else
	{
		Engine::Internal::Components::GameObject^ value = (Engine::Internal::Components::GameObject^)fieldInfo->GetValue(obj);

		temp = CastStringToNative(value->name + " - (" + GetAccessRoute(value) + ")###" + fieldInfo->Name);
	}

	if (ImGui::Button(temp.c_str()))
	{
		if (!selectionLock)
		{
			selectionLock = true;
		}
		else
		{
			if (selectionObject->GetType() == fieldInfo->FieldType || fieldInfo->FieldType->IsAssignableFrom(selectionObject->GetType()))
			{
				fieldInfo->SetValue(obj, selectionObject);
			}
			else
			{
				fieldInfo->SetValue(obj, nullptr);
			}

			selectionLock = false;
		}
	}
}


void InstanceReferenceEditor(System::Object^ obj, System::Reflection::FieldInfo^ fieldInfo)
{
	Type^ fieldType = fieldInfo->FieldType;
	Type^ innerType = fieldType->GetGenericArguments()[0];
	Object^ boxedValue = fieldInfo->GetValue(obj);

	System::Reflection::PropertyInfo^ instanceProp = fieldType->GetProperty("Instance");

	Object^ currentInstance = nullptr;
	if (boxedValue != nullptr)
		currentInstance = instanceProp->GetValue(boxedValue);

	std::string label;
	if (boxedValue == nullptr || currentInstance == nullptr)
	{
		label = CastStringToNative(
			"NOT ASSIGNED - (NULL)###" + fieldInfo->Name);
	}
	else
	{
		Engine::Internal::Components::GameObject^ go =
			safe_cast<Engine::Internal::Components::GameObject^>(currentInstance);

		label = CastStringToNative(
			go->name + " - (" + GetAccessRoute(go) + ")###" + fieldInfo->Name);
	}

	if (ImGui::Button(label.c_str()))
	{
		if (!selectionLock)
		{
			selectionLock = true;
		}
		else
		{
			Engine::Internal::Components::GameObject^ selectedObject = selectionObject;

			bool assignable = selectedObject != nullptr &&
				(innerType->IsAssignableFrom(selectedObject->GetType()));

			instanceProp->SetValue(
				boxedValue,
				assignable ? selectedObject : nullptr
			);

			fieldInfo->SetValue(obj, boxedValue);

			selectionLock = false;
		}
	}
}

inline void EnableFBXConverter(std::string fbxFilePath)
{
	showFbxConverter = true;
	fbxFile = fbxFilePath;
}

inline void OnFBXConverted(String^ fileName)
{
	fbxConverter = new AssimpConverter(fbxFile, CastStringToNative(fileName), exportId);

	delete fbxConverter;
}

inline void RenderFBXConverter()
{
	if(showFbxConverter)
		ImGui::OpenPopup("FBX Converter");

	bool conversion = false;

	if (ImGui::BeginPopupModal("FBX Converter", &showFbxConverter))
	{
		ImGui::Text("Export Format");

		if (ImGui::BeginCombo("###EXPORT_FORMAT", exportDesc.c_str()))
		{
			for (int x = 0; x < getAssimpExporters(); x++)
			{
				if (ImGui::Selectable(getAssimpExporterDescription(x)))
				{
					exportDesc = std::string(getAssimpExporterDescription(x));
					exportId = std::string(getAssimpExporterId(x));
				}
			}

			ImGui::EndCombo();
		}

		if (ImGui::Button("Convert"))
		{
			conversion = true;
			showFbxConverter = false;
		}

		ImGui::EndPopup();
	}

	if (conversion)
	{
		auto fileExplorer = Singleton<Engine::Editor::Gui::fileExplorer^>::Instance;

		fileExplorer->SetWindowName("Convert FBX File");
		fileExplorer->setExplorerMode(Engine::Editor::Gui::explorerMode::Save);
		fileExplorer->Open();

		fileExplorer->OnCompleted(gcnew Engine::Editor::Gui::onFileSelected(&OnFBXConverted));
	}
}