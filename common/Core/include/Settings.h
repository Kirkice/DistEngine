/* $Header: /common/Core/include/Settings.h				8/25/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Settings.h											      *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once

#include "PCH.h"
#include "../../DistMath/include/DistMath.h"
#include "Serialization.h"
#include "Containers.h"

namespace core
{
	class FloatSetting;
	class IntSetting;
	class BoolSetting;
	class EnumSetting;
	class DirectionSetting;
	class OrientationSetting;
	class ColorSetting;
	class Button;

	enum class SettingType
	{
		Float = 0,
		Int = 1,
		Bool = 2,
		Enum = 3,
		Direction = 4,
		Orientation = 5,
		Color = 6,
		Button = 7,

		Invalid,
		NumTypes = Invalid
	};

	enum class ConversionMode
	{
		None = 0,
		Square = 1,
		SquareRoot = 2,
		DegToRadians = 3,
	};

	enum class ColorUnit
	{
		None = 0,
		Luminance = 1,
		Illuminance = 2,
		LuminousPower = 3,
		EV100 = 4,
	};

	enum class DirectionInputMode
	{
		Cartesian = 0,
		Spherical
	};

	// Base class for all setting types
	class Setting
	{

	protected:

		SettingType type = SettingType::Invalid;
		void* data = nullptr;
		std::string name;
		std::string group;
		std::string label;
		std::string helpText;
		bool changed = false;
		bool initialized = false;
		bool visible = true;

		void Initialize(SettingType type, void* data, const char* name,
			const char* group, const char* label, const char* helpText);

	public:

		Setting();

		virtual void Update(const Float4x4& viewMatrix) = 0;

		virtual void SetReadOnly(bool readOnly);
		void SetEditable(bool editable);
		void SetHidden(bool hidden);
		void SetVisible(bool visible);
		void SetLabel(const char* label);

		FloatSetting& AsFloat();
		IntSetting& AsInt();
		BoolSetting& AsBool();
		EnumSetting& AsEnum();
		DirectionSetting& AsDirection();
		OrientationSetting& AsOrientation();
		ColorSetting& AsColor();
		Button& AsButton();

		bool Changed() const;
		bool Visible() const;
		const std::string& Name() const;
		const std::string& Group() const;

		uint64 SerializedValueSize();

		template<typename TSerializer> void SerializeValue(TSerializer& serializer)
		{
			Assert_(initialized);
			if (type == SettingType::Float)
				AsFloat().SerializeValue(serializer);
			else if (type == SettingType::Int)
				AsInt().SerializeValue(serializer);
			else if (type == SettingType::Bool)
				AsBool().SerializeValue(serializer);
			else if (type == SettingType::Enum)
				AsEnum().SerializeValue(serializer);
			else if (type == SettingType::Direction)
				AsDirection().SerializeValue(serializer);
			else if (type == SettingType::Orientation)
				AsOrientation().SerializeValue(serializer);
			else if (type == SettingType::Color)
				AsColor().SerializeValue(serializer);
		}
	};

	// 32-bit float setting
	class FloatSetting : public Setting
	{

	private:

		float val;
		float oldVal;
		float minVal;
		float maxVal;
		float step;
		ConversionMode conversionMode = ConversionMode::None;
		float conversionScale = 1.0f;

	public:

		FloatSetting();

		void Initialize(const char* name, const char* group,
			const char* label, const char* helpText, float initialVal,
			float minVal, float maxVal, float step, ConversionMode conversionMode,
			float conversionScale);

		virtual void Update(const Float4x4& viewMatrix) override;

		float Value() const;
		float RawValue() const;
		void SetValue(float newVal);
		operator float();

		float MinValue() const { return minVal; };
		float MaxValue() const { return maxVal; };

		template<typename TSerializer> void SerializeValue(TSerializer& serializer)
		{
			Assert_(initialized);
			SerializeItem(serializer, val);
			if (serializer.IsReadSerializer())
				val = Clamp(val, minVal, maxVal);
		}
	};

	// 32-bit integer setting
	class IntSetting : public Setting
	{

	private:

		int32 val;
		int32 oldVal;
		int32 minVal;
		int32 maxVal;

	public:

		IntSetting();

		void Initialize(const char* name, const char* group,
			const char* label, const char* helpText, int32 initialVal,
			int32 minVal, int32 maxVal);

		virtual void Update(const Float4x4& viewMatrix) override;

		int32 Value() const;
		void SetValue(int32 newVal);
		operator int32();

		int32 MinValue() const { return minVal; };
		int32 MaxValue() const { return maxVal; };

		template<typename TSerializer> void SerializeValue(TSerializer& serializer)
		{
			Assert_(initialized);
			SerializeItem(serializer, val);
			if (serializer.IsReadSerializer())
				val = Clamp(val, minVal, maxVal);
		}
	};

	// Boolean setting
	class BoolSetting : public Setting
	{

	private:

		bool32 val;
		bool32 oldVal;

	public:

		BoolSetting();

		void Initialize(const char* name, const char* group,
			const char* label, const char* helpText, bool32 initialVal);

		virtual void Update(const Float4x4& viewMatrix) override;

		bool32 Value() const;
		void SetValue(bool32 newVal);
		operator bool32();

		template<typename TSerializer> void SerializeValue(TSerializer& serializer)
		{
			Assert_(initialized);
			SerializeItem(serializer, val);
		}
	};

	// Enumeration setting
	class EnumSetting : public Setting
	{

	protected:

		uint32 val = 0;
		uint32 oldVal = 0;
		uint32 numValues = 0;
		uint32 numValuesClamp = 0;
		const char** valueLabels = nullptr;

	public:

		EnumSetting();

		void Initialize(const char* name, const char* group,
			const char* label, const char* helpText, uint32 initialVal,
			uint32 numValues, const char** valueLabels);

		virtual void Update(const Float4x4& viewMatrix) override;

		uint32 Value() const;
		void SetValue(uint32 newVal);
		void ClampNumValues(uint32 num);

		operator uint32();

		template<typename TSerializer> void SerializeValue(TSerializer& serializer)
		{
			Assert_(initialized);
			SerializeItem(serializer, val);
			if (serializer.IsReadSerializer())
				val = std::min(val, numValues - 1);
		}
	};

	// Templated enumeration setting
	template<typename T> class EnumSettingT : public EnumSetting
	{

	public:

		EnumSettingT()
		{
		}

		void Initialize(const char* name_, const char* group_,
			const char* label_, const char* helpText_, T initialVal,
			uint32 numValues_, const char** valueLabels_)
		{
			EnumSetting::Initialize(name_, group_, label_, helpText_,
				uint32(initialVal), numValues_, valueLabels_);
		}

		operator T()
		{
			return T(val);
		}

		void SetValue(T newVal)
		{
			EnumSetting::SetValue(uint32(newVal));
		}
	};

	// 3D direction setting
	class DirectionSetting : public Setting
	{

	private:

		Vector3 val;
		Vector3 oldVal;
		Vector2 spherical;

		std::string buttonName;
		std::string childName;
		Vector2 lastDragDelta;
		bool wasDragged = false;
		bool convertToViewSpace = false;
		DirectionInputMode inputMode = DirectionInputMode::Cartesian;

	public:

		DirectionSetting();

		void Initialize(const char* name, const char* group, const char* label, const char* helpText,
			Vector3 initialVal, bool convertToViewSpace);

		virtual void Update(const Float4x4& viewMatrix) override;

		Vector3 Value() const;
		void SetValue(Vector3 newVal);
		operator Vector3();

		template<typename TSerializer> void SerializeValue(TSerializer& serializer)
		{
			Assert_(initialized);
			SerializeItem(serializer, val);
		}
	};

	// Quaternion orientation setting
	class OrientationSetting : public Setting
	{

	private:

		Quaternion val;
		Quaternion oldVal;

		std::string buttonName;
		std::string childName;
		Vector2 lastDragDelta;
		bool wasDragged = false;
		bool convertToViewSpace = false;

	public:

		OrientationSetting();

		void Initialize(const char* name, const char* group, const char* label, const char* helpText,
			Quaternion initialVal, bool convertToViewSpace);

		virtual void Update(const Float4x4& viewMatrix) override;

		Quaternion Value() const;
		void SetValue(Quaternion newVal);
		operator Quaternion();

		template<typename TSerializer> void SerializeValue(TSerializer& serializer)
		{
			Assert_(initialized);
			SerializeItem(serializer, val);
		}
	};

	// RGB color setting
	class ColorSetting : public Setting
	{

	private:

		Vector3 val;
		Vector3 oldVal;

		FloatSetting intensity;
		ColorUnit units = ColorUnit::None;
		bool hdr = false;


	public:

		ColorSetting();

		void Initialize(const char* name, const char* group,
			const char* label, const char* helpText, Vector3 initialVal,
			bool hdr, float minIntensity, float maxIntensity, float step,
			ColorUnit units);

		virtual void Update(const Float4x4& viewMatrix) override;
		virtual void SetReadOnly(bool readOnly) override;

		Vector3 Value() const;
		void SetValue(Vector3 newVal);
		operator Vector3();

		float Intensity() const;
		void SetIntensity(float newIntensity);
		void SetIntensityVisible(bool visible);
		void SetUnits(ColorUnit newUnits);

		template<typename TSerializer> void SerializeValue(TSerializer& serializer)
		{
			Assert_(initialized);
			SerializeItem(serializer, val);
			if (hdr)
				intensity.SerializeValue(serializer);
		}
	};

	// Button
	class Button : public Setting
	{

	private:

		bool wasPressed;
		bool currentlyPressed;

	public:

		Button();

		void Initialize(const char* name, const char* group,
			const char* label, const char* helpText);

		virtual void Update(const Float4x4& viewMatrix) override;

		bool Pressed() const { return currentlyPressed; }
		operator bool() const { return Pressed(); }
	};

	// Container for settings (essentially wraps a tweak bar)
	class SettingsContainer
	{

	private:

		struct SettingsGroup
		{
			GrowableList<Setting*> Settings;
			std::string Name;
			bool Expanded = true;
		};

		FixedList<SettingsGroup> groups;
		bool initialized = false;
		bool opened = true;

	public:

		SettingsContainer();
		~SettingsContainer();

		void Initialize(uint64 numGroups);

		void Update(uint32 displayWidth, uint32 displayHeight, const Float4x4& viewMatrix);

		Setting* FindSetting(const char* name);

		// Add a new group
		void AddGroup(const char* name, bool expanded);

		// Add new settings
		void AddSetting(Setting* setting);

		void SetWindowOpened(bool windowOpened) { opened = windowOpened; }
	};
}