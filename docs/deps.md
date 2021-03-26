# Dependencies

Option value definition may have one or more dependencies on other option value
definitions within the same model or across included models. The supported types
of dependencies are presented below. Dependent types are addressed using a local
JSONPath path.

## Requirement dependency

When options are mentioned in this type of dependency, their arity is promoted
to `required`.

```json
"deps": [ { "require": [ "@.A", ... ] } ]
```

## Disable dependency

When options are mentioned in this type of dependency, their use is disabled.

```json
"deps": [ { "disable": [ "@.A", ... ] } ]
```

## Value-guarded dependency

Value-guarded dependencies are activated only when the option has one of the
value specified in the `when` list.

```json
"deps": [ { "when": [ v0, v1 ], "require": [ "@.A", ... ] } ]
```

```json
"deps": [ { "when": [ v0, v1 ], "disable": [ "@.A", ... ] } ]
```

## Constraint-guarded dependency

Constraint-guarded dependencies enforce the final value of the dependent type.

```json
"deps": [ { "require": [ "@.A", ... ], "either": [ V1, V2 ] } ]
```

```json
"deps": [ { "require": [ "@.A", ... ], "range": "[ V1, V2 ]" } ]
```

## Mixing dependencies

```json
"deps": [
  { "require": [ "@.A", ... ] },
  { "when": [ V ], "require": [ "@.A", ... ] }
]
```

