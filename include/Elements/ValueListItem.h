#pragma once

#include <tesla.hpp>

#include <string>
#include <functional>

class ValueListItem : public tsl::elm::ListItem
{
public:
  ValueListItem(std::string text, const std::vector<std::string> values, int defaultPos, const std::string extData);
  ~ValueListItem();

  tsl::elm::Element *requestFocus(Element *oldFocus, tsl::FocusDirection direction) override;

  virtual void layout(u16 parentX, u16 parentY, u16 parentWidth, u16 parentHeight) override;

  int getCurValue()
  {
    return this->m_curValue;
  }
  void setCurValue(int value) { this->m_curValue = value; }

  const std::string getExtData() { return this->extdata; }

  const std::vector<std::string> getValues() { return this->m_values; }

  bool onClick(u64 keys) override;

  void setStateChangeListener(std::function<void(const std::vector<std::string>, int, std::string)> valueChangeListener) { this->m_valueChangeListener = valueChangeListener; }

private:
  const std::vector<std::string> m_values;
  int m_curValue;
  std::function<void(const std::vector<std::string>, int, std::string)> m_valueChangeListener = nullptr;
  const std::string extdata;
};