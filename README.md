# Event Systems: A Comparative Study

This repository is a comparative exploration of event-system architectures in modern C++, focused on design tradeoffs, maintainability, and evolution from simple callback models to generation-driven static contracts.

## Implementations

- [PubSub](PubSub/README.md)
- [Dynamic Event System](DynamicEventSystems/README.md)
- [Static Event System](StaticEventSystem/README.md)

## Build

Each implementation has its own build and run instructions in its local README.

## Notes

- PlantUML sources are stored next to each implementation under its `diagrams/` folder.
- GitHub does not render PlantUML blocks in Markdown out of the box.

## Suggested Reading Path

1. Start with [PubSub](PubSub/README.md) for the smallest mental model.
2. Continue with [Dynamic Event System](DynamicEventSystems/README.md) for runtime flexibility.
3. Finish with [Static Event System](StaticEventSystem/README.md) for generation-based strict interfaces and startup safety.

## License

MIT License - See LICENSE file for details

## Contributing

Contributions welcome! Please open an issue first to discuss proposed changes.
