# ⚙️ CSigma Architecture — Event System & Intelligent Translation Engine

### Overview

**CSigma** is a C++ framework currently under active development, designed to provide a **lightweight, high-performance, and fully native environment** for building modern graphical interfaces.  
It focuses on two core technical pillars:

1. A **grouped event management system**, inspired by Win32 for optimal development and runtime control.  
2. An **intelligent, AI-assisted translation engine**, capable of switching the interface language instantly, with quality-controlled translation in real time.

---

## 🧩 1. Event Management System (Win32-inspired, Modernized)

### Concept

CSigma uses an event-driven model based on **direct message dispatching**, following the same efficiency principles as the Win32 API.  
Each user interface element registers one or more **actions** that respond to system messages such as clicks, key presses, or focus changes.

```cpp
CSSECMAN::addAction(ControlID, HandlerFunction, ArgsCount, ...);
```

Example handler:

```cpp
void onClickExample(CSARGS Args) {
    if (UINT(Args) == WM_LBUTTONUP)
        MessageBoxW(NULL, L"Action executed successfully!", L"CSigma", MB_OK);
}
```

This design provides developers with **complete control** over user interaction while maintaining **very low latency** and **predictable behavior**.

---

### Performance Profile

The CSigma event dispatcher operates with **constant-time callback resolution**, without any reflection, signal-slot translation, or virtual indirection.  
This yields near-native performance comparable to pure Win32 message loops, even for complex interfaces.

| Framework | Event Model | Average Latency | Control Granularity | Thread-Safe | Overhead |
|------------|--------------|------------------|----------------------|--------------|-----------|
| **Win32 API** | `WndProc` message callback | ⚡ Very low | 🔧 Full | Manual | Minimal |
| **Qt** | Signal/Slot system | 🐢 Higher | 🟡 Medium | Partial | Moderate |
| **wxWidgets** | Event tables | ⚙️ Moderate | 🟡 Medium | Partial | Moderate |
| **ImGui** | Centralized polling | ⚡ Very low | 🟠 Limited | ✅ Yes | Low |
| **CSigma** | Grouped message callbacks | ⚡ Very low | 🔧 Full | ✅ Managed | Minimal |

**Summary:**  
CSigma offers the **direct responsiveness of Win32**, with a more organized and modular event binding model, allowing developers to retain **fine-grained runtime control** without framework-imposed abstraction layers.

---

### Technical Highlights

- **Direct mapping** to native `WM_*` events (no emulation or abstraction overhead)  
- **Section-based event grouping**, enabling clear modular UI design  
- **Unified callback interface (`CSARGS`)** for all control types  
- **Asynchronous-safe** — threads can post events to the UI manager safely  
- **No dependency on runtime reflection** or meta-object systems  

This architecture ensures that **response times remain deterministic**, even under heavy UI activity or parallel task execution.

---

## 🌐 2. Intelligent Translation Engine (CSLANGMAN)

### Concept

CSigma introduces a **real-time translation system** that goes beyond traditional static localization.  
Through `CSLANGMAN`, any CSigma-based application can switch its interface language **instantly**, without reloading the UI or restarting the program.  

This translation is **AI-assisted**, meaning the engine can connect to a translation backend (local or remote) to generate accurate multilingual text dynamically.  
Quality control mechanisms ensure that translations remain consistent across updates and can be cached for offline use.

---

### Example Usage

```cpp
CSLANGMAN::translateAppStrings();
```

All visible elements (titles, buttons, tooltips, dialogs) are updated automatically, regardless of their hierarchy or origin.

---

### Comparison with Other Frameworks

| Framework | Translation System | Dynamic Runtime Switching | AI Integration | Control Granularity | Notes |
|------------|-------------------|----------------------------|----------------|----------------------|--------|
| **Qt** | Static `.ts` files + `QTranslator` | ⚠️ Partial (requires reload) | ❌ None | 🟡 Widget-level | Manual reload of UI |
| **wxWidgets** | `.mo` files via `wxLocale` | ❌ No | ❌ None | ⚙️ Application-level | Static language files |
| **GTK / gettext** | `.po` files | ❌ No | ❌ None | 🟡 Moderate | Requires reinitialization |
| **ImGui / SDL** | Manual string replacement | ❌ No | ❌ None | 🔧 Full (manual) | Developer-managed |
| **CSigma** | `CSLANGMAN` in-memory map | ✅ Yes (instant) | ✅ Built-in or external | 🔧 Full | Real-time, quality-controlled translation |

---

### Technical Highlights

- **In-memory multilingual mapping** (no file reloads or recompilation)  
- **AI-assisted translation layer** for new or missing strings  
- **Real-time propagation** — all UI controls are refreshed on language change  
- **Quality validation** — checks for consistency and grammar accuracy across translations  
- **Fallback mechanism** — automatic revert to default language when a translation key is missing  

> In practical terms, CSigma can re-render its entire interface in another language within milliseconds, maintaining layout and visual integrity.

---

## 🧠 3. Development Status

CSigma is currently **in active development**.  
The architecture, event dispatcher, and translation system are functional and evolving toward greater modularity and cross-platform compatibility.  

Ongoing work includes:
- Expanding supported UI controls  
- Extending AI translation backends (for offline and hybrid modes)  
- Improving integration between `CSSECMAN` and `CSLANGMAN` for dynamic content handling  

Even at this stage, the framework already demonstrates **robust real-time responsiveness** and a **unique approach to intelligent multilingual interfaces** in C++.

---

## 🏁 4. Summary

| Feature | Qt | wxWidgets | ImGui | Win32 | **CSigma** |
|----------|----|-----------|--------|--------|-------------|
| C++ Native | ✅ | ✅ | ✅ | ✅ | ✅ |
| Lightweight Runtime | ⚠️ | ⚠️ | ✅ | ✅ | ✅ |
| Event Responsiveness | 🟡 | 🟡 | ✅ | ✅ | ✅ |
| Dynamic Translation | ⚠️ Partial | ❌ | ❌ | ❌ | ✅ Instant |
| AI Translation | ❌ | ❌ | ❌ | ❌ | ✅ Built-in |
| Thread-safe Messaging | 🟡 | 🟡 | ✅ | ⚙️ Manual | ✅ Managed |
| Real-time UI Refresh | ⚠️ Limited | ⚠️ | ✅ | ⚙️ | ✅ Full |
| Open Development | ✅ | ✅ | ✅ | ✅ | 🚧 In Progress |

---

### In summary
> **CSigma** redefines native C++ interface development by combining a **Win32-level event system** with an **AI-driven, real-time translation engine**.  
> It provides deterministic performance, complete runtime control, and the capability to localize any application instantly in any language — all within a clean, modern, and modular architecture.
