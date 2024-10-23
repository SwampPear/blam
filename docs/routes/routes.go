package routes

import (
    "github.com/gofiber/fiber/v2"
    "github.com/SwampPear/blam/docs/handlers"
)

func SetupRoutes(app *fiber.App) {
	app.Get("/", handlers.Index)
	app.Get("/bloop", handlers.Bloop)
	app.Get("/compiler", handlers.Compiler)
	app.Get("/comments", handlers.Comments)
	app.Get("/data_types", handlers.DataTypes)
	app.Get("/functions", handlers.Functions)
	app.Get("/variables", handlers.Variables)
	app.Get("/control_flow", handlers.ControlFlow)
	app.Get("/structs", handlers.Structs)
	app.Get("/errors", handlers.Errors)
	app.Get("/ownership", handlers.Ownership)
	app.Use(handlers.Error404)
}