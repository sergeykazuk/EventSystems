# Event Systems: A Comparative Study

This repository serves as a technical portfolio demonstrating modular, scalable architectural patterns for complex environments. These patterns are inspired by my experience delivering Production-Ready Software and SOPs for Tier-1 automotive brands including Audi and VW.

In safety-critical HMI and ADAS development, decoupling communication is not just a preference, it is a requirement for traceability and ISO 26262 compliance.

## Why This Repository

This project compares three architectural styles for event-driven communication in C++:

- PubSub baseline for simple decoupling.
- Dynamic event system for runtime flexibility.
- Static generated event system for stronger contracts and startup determinism.

The intent is not to present one universally "best" solution, but to show engineering tradeoffs and architectural evolution from simple to robust.

## At a Glance

| Approach | Best For | Main Strength | Main Tradeoff |
|---|---|---|---|
| PubSub | Small/medium apps, rapid prototyping | Very low entry cost and simple model | Weaker contract guarantees and lifecycle control |
| Dynamic Event System | Extensible systems with runtime event variability | Runtime flexibility and decoupled async dispatch | More runtime complexity and registration overhead |
| Static Event System | Systems that need strict interfaces and predictable startup | Compile-time contracts + generated APIs | Generator/tooling complexity |

## Comparison Summary

| Dimension | PubSub | Dynamic | Static |
|---|---|---|---|
| Type safety | Medium | Medium | High |
| Runtime flexibility | Medium | High | Low-Medium |
| Startup determinism | Low | Medium | High |
| Tooling complexity | Low | Medium | High |
| Best portfolio signal | Basics and clarity | Runtime architecture | Advanced system design |

## Implementations

- [PubSub](PubSub.md)
- [Dynamic Event System](DynamicES.md)
- [Static Event System](StaticES.md)

## Build

Please refer to project specific readmes.

## Notes

- PlantUML sources are stored next to each implementation under its `diagrams/` folder.
- GitHub does not render PlantUML blocks in Markdown out of the box.

## Suggested Reading Path

1. Start with [PubSub](PubSub.md) for the smallest mental model.
2. Continue with [Dynamic Event System](DynamicES.md) for runtime flexibility.
3. Finish with [Static Event System](StaticES.md) for generation-based strict interfaces and startup safety.

## License

MIT License - See LICENSE file for details

## Contributing

Contributions welcome! Please open an issue first to discuss proposed changes.
