#include <switch.h>
#include <tesla.hpp>

#include <Elements/ValueListItem.h>

#include <taunt_toggles.h>

#include <vector>
#include <cstring>

struct DebugInfoAttachProcess
{
  u64 program_id;
  u64 process_id;
  char name[0xC];
  u32 flags;
  u32 user_exception_context_address; /* 5.0.0+ */
};

enum DebugEvent : u32
{
  DebugEvent_AttachProcess = 0,
  DebugEvent_AttachThread = 1,
  DebugEvent_ExitProcess = 2,
  DebugEvent_ExitThread = 3,
  DebugEvent_Exception = 4,
};
struct DebugEventInfo
{
  DebugEvent type;
  u32 flags;
  u64 thread_id;
  DebugInfoAttachProcess info;
};

tsl::element::ToggleListItem *toggleItem = nullptr;
std::vector<ValueListItem *> valueListItems;

u64 pidSmash = 0;
u64 pidSalty = 0;

void applyChanges()
{
  for (ValueListItem *item : valueListItems)
  {
    int value = item->getCurValue();
    std::string extData = item->getExtData();
    if (extData == "shield")
    {
      menu.SHIELD_STATE = value;
    }
    if (extData == "mash")
    {
      menu.MASH_STATE = value;
    }
    if (extData == "ledge")
    {
      menu.LEDGE_STATE = value;
    }
    if (extData == "tech")
    {
      menu.TECH_STATE = value;
    }
    if (extData == "defensive")
    {
      menu.DEFENSIVE_STATE = value;
    }
    if (extData == "di")
    {
      menu.DI_STATE = value;
    }
  }
  Result rc;
  Handle debug;

  u64 pid = 0;
  pmdmntGetProcessId(&pid, 0x01006A800016E000);
  if (pid != 0)
  {
    rc = svcDebugActiveProcess(&debug, pid);
    if (R_SUCCEEDED(rc))
    {
      u64 menu_addr = 0;
      FILE *f = fopen("sdmc:/SaltySD/training_modpack.log", "r");

      char buffer[100];
      if (f)
      {
        int read_bytes = fread(buffer, 1, 100, f);
        fclose(f);
        buffer[read_bytes] = '\0';
        menu_addr = strtoul(buffer, NULL, 16);
      }
      if (menu_addr != 0)
      {
        rc = svcWriteDebugProcessMemory(debug, &menu, (u64)menu_addr, sizeof(menu));
      }
      svcCloseHandle(debug);
    }
  }
  FILE *f = fopen("sdmc:/SaltySD/training_modpack_menu.conf", "w");
  if (f)
  {
    fwrite(static_cast<void *>(&menu), sizeof(menu), 1, f);
    fclose(f);
  }
}

class GuiMain : public tsl::Gui
{
public:
  GuiMain()
  {
  }
  ~GuiMain()
  {
  }

  // Called on UI creation
  virtual tsl::Element *createUI()
  {
    auto *rootFrame = new tsl::element::Frame();

    // A CustomDrawer element that allows for direct draw actions to the framebuffer

    auto *header = new tsl::element::CustomDrawer(0, 0, 100, FB_WIDTH, [](u16 x, u16 y, tsl::Screen *screen) {
      screen->drawString("UltimateTrainingModpack", false, 20, 50, 30, tsl::a(0xFFFF));
      screen->drawString("Configurator", false, 20, 68, 15, tsl::a(0xFFFF));
    });
    rootFrame->addElement(header);

    bool boot = 0;

    FILE *f = fopen("sdmc:/atmosphere/contents/0100000000534C56/flags/boot2.flag", "r");
    if (f)
    {
      boot = 1;
      fclose(f);
      f = nullptr;
    }

    auto list = new tsl::element::List();
    auto saltyBootItem = new tsl::element::ToggleListItem("SaltyNX boot2.flag", boot);

    saltyBootItem->setStateChangeListener([](bool state) {
      if (state == 0)
      {
        std::remove("sdmc:/atmosphere/contents/0100000000534C56/flags/boot2.flag");
      }
      else
      {
        FILE *f = fopen("sdmc:/atmosphere/contents/0100000000534C56/flags/boot2.flag", "w");
        fclose(f);
      }
    });
    Result rc;
    Handle debug;

    list->addItem(saltyBootItem);

    if (pidSalty != 0)
    {
      if (pidSmash != 0)
      {
        rc = svcDebugActiveProcess(&debug, pidSmash);
        if (R_SUCCEEDED(rc))
        {
          svcCloseHandle(debug);

          toggleItem = new tsl::element::ToggleListItem("Hitbox Visualization", menu.HITBOX_VIS);
          list->addItem(toggleItem);

          ValueListItem *shieldItem = new ValueListItem("Shield Options", shield_items, menu.SHIELD_STATE, "shield");
          list->addItem(shieldItem);
          valueListItems.push_back(shieldItem);

          ValueListItem *mashItem = new ValueListItem("Mash Toggles", mash_items, menu.MASH_STATE, "mash");
          list->addItem(mashItem);
          valueListItems.push_back(mashItem);

          ValueListItem *ledgeItem = new ValueListItem("Ledge Option", ledge_items, menu.LEDGE_STATE, "ledge");
          list->addItem(ledgeItem);
          valueListItems.push_back(ledgeItem);

          ValueListItem *techItem = new ValueListItem("Tech Options", tech_items, menu.TECH_STATE, "tech");
          list->addItem(techItem);
          valueListItems.push_back(techItem);

          ValueListItem *defensiveItem = new ValueListItem("Defensive Options", defensive_items, menu.DEFENSIVE_STATE, "defensive");
          list->addItem(defensiveItem);
          valueListItems.push_back(defensiveItem);

          ValueListItem *diItem = new ValueListItem("Set DI", di_items, menu.DI_STATE, "di");
          list->addItem(diItem);
          valueListItems.push_back(diItem);

          rootFrame->addElement(list);
        }
        else
        {
          tsl::element::CustomDrawer *warning = new tsl::element::CustomDrawer(0, 0, 100, FB_WIDTH, [](u16 x, u16 y, tsl::Screen *screen) {
            screen->drawString("\uE150", false, 180, 250, 90, tsl::a(0xFFFF));
            screen->drawString("Could not debug process memory", false, 110, 340, 25, tsl::a(0xFFFF));
          });

          rootFrame->addElement(warning);
        }
      }
      else
      {
        tsl::element::CustomDrawer *warning = new tsl::element::CustomDrawer(0, 0, 100, FB_WIDTH, [](u16 x, u16 y, tsl::Screen *screen) {
          screen->drawString("\uE150", false, 180, 400, 90, tsl::a(0xFFFF));
          screen->drawString("Smash not running", false, 110, 500, 25, tsl::a(0xFFFF));
        });

        rootFrame->addElement(warning);
      }
    }
    else
    {
      tsl::element::CustomDrawer *warning = new tsl::element::CustomDrawer(0, 0, 100, FB_WIDTH, [](u16 x, u16 y, tsl::Screen *screen) {
        screen->drawString("\uE150", false, 180, 400, 90, tsl::a(0xFFFF));
        screen->drawString("SaltyNX Not Running", false, 110, 500, 25, tsl::a(0xFFFF));
      });

      rootFrame->addElement(warning);
    }
    rootFrame->addElement(list);
    return rootFrame;
  }
  // Called once per frame
  virtual void
  update()
  {
  }
};

class TrainingModpackOverlay : public tsl::Overlay
{
public:
  TrainingModpackOverlay() {}
  ~TrainingModpackOverlay() {}

  // Called once right after the overlay was launched
  tsl::Gui *onSetup()
  {
    return new GuiMain();
  }

  // Called once immediately before exiting
  void onDestroy()
  {
    // Exit your services here
  }
  void onOverlayHide(tsl::Gui *gui)
  {
    applyChanges();
    gui->playOutroAnimation();
  }
  void onOverlayExit(tsl::Gui *gui)
  {
    applyChanges();
    Overlay::onOverlayExit(gui);
  }
};

// This function will get called once on startup to load the overlay
tsl::Overlay *overlayLoad()
{

  smInitialize();
  pminfoInitialize();
  pmbmInitialize();
  smExit();

  pmdmntGetProcessId(&pidSmash, 0x01006A800016E000);

  u64 pids[256];
  u32 num_pids;
  svcGetProcessList(&num_pids, pids, 256);

  struct ProcessReport
  {
    uint64_t process_id;
    uint32_t result;
    uint64_t title_id;
    char process_name[12];
    uint32_t mmu_flags;
  };

  for (uint32_t i = 0; i < num_pids; i++)
  {
    Handle handle;
    if (R_SUCCEEDED(svcDebugActiveProcess(&handle, pids[i])))
    {
      DebugEventInfo event;
      while (R_SUCCEEDED(svcGetDebugEvent(reinterpret_cast<u8 *>(&event), handle)))
      {
        if (event.type == DebugEvent_AttachProcess)
        {
          auto name = event.info.name;
          if (!strcmp("SaltySD", name))
          {
            pidSalty = pids[i];
          }
        }
      }
      svcCloseHandle(handle);
    }
  }

  FILE *g = fopen("sdmc:/SaltySD/training_modpack_menu.conf", "r");
  if (g)
  {
    fread(static_cast<void *>(&menu), sizeof(menu), 1, g);
    fclose(g);
  }
  else
  {
    g = fopen("sdmc:/SaltySD/training_modpack_menu.conf", "w");
    if (g)
    {
      fwrite(static_cast<void *>(&menu), sizeof(menu), 1, g);
      fclose(g);
    }
  }

  return new TrainingModpackOverlay();
}